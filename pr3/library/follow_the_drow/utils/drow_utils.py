"""
This file is totally based on research papaer about DROW.
Here the link to the paper can be found:
https://arxiv.org/abs/1603.02636
The utility functions present here can be found in paper's GitHub:
https://github.com/VisualComputingInstitute/DROW/blob/master/v2/utils/__init__.py
"""

from collections import defaultdict

from scipy.ndimage import maximum_filter
from scipy.spatial.distance import cdist
from scipy.optimize import linear_sum_assignment

from numpy import zeros, arctan, arctan2, square, add, sin, cos, logical_not, concatenate, linspace, sum, mean, argmin, argsort, array, where, full, full_like, arange, clip, unique, radians, float32, int64, uint32, nan, c_, r_
from numpy.typing import NDArray

from cv2 import resize, GaussianBlur, INTER_AREA, INTER_LINEAR

laser_measures = 450
laser_increment = radians(0.5)
laser_FoV = (laser_measures - 1) * laser_increment
laser_minimum = -laser_FoV * 0.5
laser_maximum = laser_FoV * 0.5


def cutout(scans, odoms, number, win_sz=1.66, thresh_dist=1, nsamp=48, UNK=29.99, laserIncrement=laser_increment):
    """ TODO: Probably we can still try to clean this up more.
    This function here only creates a single cut-out; for training,
    we'd want to get a batch of cutouts from each seq (can vectorize) and for testing
    we'd want all cutouts for one scan, which we can vectorize too.
    But ain't got time for this shit!

    Args:
    - scans: (T,N) the T scans (of scansize N) to cut out from, `T=-1` being the "current time".
    - out: None or a (T,nsamp) buffer where to store the cutouts.
    """
    T, N = scans.shape
    out = zeros((number, T, nsamp), float32)

    for ipoint in range(number):
        # Compute the size (width) of the window at the last time index:
        z = scans[-1,ipoint]
        half_alpha = float(arctan(0.5*win_sz/z))

        # Pre-allocate some buffers
        SCANBUF = full(N + 1, UNK, float32)  # Pad by UNK for the border-padding by UNK.
        for t in range(T):
            # If necessary, compute the odometry of the current time relative to the "key" one.
            odom_x, odom_y, odom_a = map(float, odoms[t]["xya"] - odoms[-1]["xya"])

            # Compute the start and end indices of points in the scan to be considered.
            start = int(round(ipoint - half_alpha/laserIncrement - odom_a/laserIncrement))
            end = int(round(ipoint + half_alpha/laserIncrement - odom_a/laserIncrement))

            # Now compute the list of indices at which to take the points,
            # using -1/end to access out-of-bounds which has been set to UNK.
            support_points = arange(start, end+1)
            support_points.clip(-1, len(SCANBUF)-1, out=support_points)

            # Write the scan into the buffer which has UNK at the end and then sample from it.
            SCANBUF[:-1] = scans[t]
            cutout = SCANBUF[support_points]

            # Now we do the resampling of the cutout to a fixed number of points. We can do it two ways:
            # In the other case, we have a somewhat distorted world-view as the x-indices
            # correspond to angles and the values to z-distances (radii) as in original DROW.
            # The advantage here is we can use the much faster OpenCV resizing functions.
            interp = INTER_AREA if nsamp < len(cutout) else INTER_LINEAR
            resized = resize(cutout[None], (nsamp,1), interpolation=interp)

            # Clip things too close and too far to create the "focus tunnel" since they are likely irrelevant.
            clipped = clip(resized, z - thresh_dist, z + thresh_dist)
            #fastclip_(cutouts[i], z - thresh_dist, z + thresh_dist)

            # And finally, possibly re-align according to a few different choices.
            clipped -= z

            out[ipoint][t] = clipped

    return out


def linearize(all_seqs, all_scans, all_detseqs, all_wcs, all_was, all_wps):
    lin_seqs, lin_scans, lin_wcs, lin_was, lin_wps = [], [], [], [], []
    # Loop through the "sessions" (correspond to files)
    for seqs, scans, detseqs, wcs, was, wps in zip(all_seqs, all_scans, all_detseqs, all_wcs, all_was, all_wps):
        # Note that sequence IDs may overlap between sessions!
        s2s = dict(zip(seqs, scans))
        # Go over the individual measurements/annotations of a session.
        for ds, wc, wa, wp in zip(detseqs, wcs, was, wps):
            lin_seqs.append(ds)
            lin_scans.append(s2s[ds])
            lin_wcs.append(wc)
            lin_was.append(wa)
            lin_wps.append(wp)
    return lin_seqs, array(lin_scans), lin_wcs, lin_was, lin_wps


# Convert it to flat `x`, `y`, `probs` arrays and an extra `frame` array,
# which is the index they had in the first place.
def _deep2flat(dets):
    all_x, all_y, all_p, all_frames = [], [], [], []
    for i, ds in enumerate(dets):
        for (x, y, p) in ds:
            all_x.append(x)
            all_y.append(y)
            all_p.append(p)
            all_frames.append(i)
    return array(all_x), array(all_y), array(all_p), array(all_frames)


def _prec_rec_2d(det_scores, det_coords, det_frames, gt_coords, gt_frames, gt_radii):
    """ Computes full precision-recall curves at all possible thresholds.

    Arguments:
    - `det_scores` (D,) array containing the scores of the D detections.
    - `det_coords` (D,2) array containing the (x,y) coordinates of the D detections.
    - `det_frames` (D,) array containing the frame number of each of the D detections.
    - `gt_coords` (L,2) array containing the (x,y) coordinates of the L labels (ground-truth detections).
    - `gt_frames` (L,) array containing the frame number of each of the L labels.
    - `gt_radii` (L,) array containing the radius at which each of the L labels should consider detection associations.
                      This will typically just be an np.full_like(gt_frames, 0.5) or similar,
                      but could vary when mixing classes, for example.

    Returns: (recs, precs, threshs)
    - `threshs`: (D,) array of sorted thresholds (scores), from higher to lower.
    - `recs`: (D,) array of recall scores corresponding to the thresholds.
    - `precs`: (D,) array of precision scores corresponding to the thresholds.
    """
    # This means that all reported detection frames which are not in ground-truth frames
    # will be counted as false-positives.
    # TODO: do some sanity-checks in the "linearization" functions before calling `prec_rec_2d`.
    frames = unique(r_[det_frames, gt_frames])

    det_accepted_idxs = defaultdict(list)
    tps = zeros(len(frames), dtype=uint32)
    fps = zeros(len(frames), dtype=uint32)
    fns = array([sum(gt_frames == f) for f in frames], dtype=uint32)

    precs = full_like(det_scores, nan)
    recs = full_like(det_scores, nan)
    threshs = full_like(det_scores, nan)

    indices = argsort(det_scores, kind='mergesort')  # mergesort for determinism.
    for i, idx in enumerate(reversed(indices)):
        frame = det_frames[idx]
        iframe = where(frames == frame)[0][0]  # Can only be a single one.

        # Accept this detection
        dets_idxs = det_accepted_idxs[frame]
        dets_idxs.append(idx)
        threshs[i] = det_scores[idx]

        dets = det_coords[dets_idxs]

        gts_mask = gt_frames == frame
        gts = gt_coords[gts_mask]
        radii = gt_radii[gts_mask]

        if len(gts) == 0:  # No GT, but there is a detection.
            fps[iframe] += 1
        else:              # There is GT and detection in this frame.
            not_in_radius = radii[:, None] < cdist(gts, dets)  # -> ngts x ndets, True (=1) if too far, False (=0) if may match.
            igt, idet = linear_sum_assignment(not_in_radius)

            tps[iframe] = sum(logical_not(not_in_radius[igt, idet]))  # Could match within radius
            fps[iframe] = len(dets) - tps[iframe]  # NB: dets is only the so-far accepted.
            fns[iframe] = len(gts) - tps[iframe]

        tp, fp, fn = sum(tps), sum(fps), sum(fns)
        precs[i] = tp/(fp+tp) if fp+tp > 0 else nan
        recs[i] = tp/(fn+tp) if fn+tp > 0 else nan

    return recs, precs, threshs


# Same but slightly different for the ground-truth.
def _deep2flat_gt(gts, radius):
    all_x, all_y, all_r, all_frames = [], [], [], []
    for i, gt in enumerate(gts):
        for (r, phi) in gt:
            x, y = rphi_to_xy(r, phi)
            all_x.append(x)
            all_y.append(y)
            all_r.append(radius)
            all_frames.append(i)
    return array(all_x), array(all_y), array(all_r), array(all_frames)


def laser_angles(N):
    return linspace(laser_minimum, laser_maximum, N)


def rphi_to_xy(r, phi):
    return r * -sin(phi), r * cos(phi)


def _win2global(r, phi, dx, dy):
    y = r + dy
    dphi = arctan2(dx, y)  # dx first is correct due to problem geometry dx -> y axis and vice versa.
    return y / cos(dphi), phi + dphi


def prepare_prec_rec_softmax(scans, pred_offs):
    angles = laser_angles(scans.shape[-1])[None,:]
    return rphi_to_xy(*_win2global(scans, angles, pred_offs[:,:,0], pred_offs[:,:,1]))


def _vote_avg(vx, vy, p):
    return mean(vx), mean(vy), mean(p, axis=0)


def _agnostic_weighted_vote_avg(vx, vy, p):
    weights = sum(p[:,1:], axis=1)
    norm = 1.0 / sum(weights)
    return norm * sum(weights*vx), norm * sum(weights*vy), norm * sum(weights[:,None]*p, axis=0)


def votes_to_detections(xs, ys, probas, weighted_avg=False, min_thresh=1e-5, bin_size=0.1, blur_win=21, blur_sigma=2.0, x_min=-15.0, x_max=15.0, y_min=-5.0, y_max=15.0, vote_collect_radius=0.3, retgrid=False, class_weights=None):
    '''
    Convert a list of votes to a list of detections based on Non-Max suppression.

    ` `vote_combiner` the combination function for the votes per detection.
    - `bin_size` the bin size (in meters) used for the grid where votes are cast.
    - `blur_win` the window size (in bins) used to blur the voting grid.
    - `blur_sigma` the sigma used to compute the Gaussian in the blur window.
    - `x_min` the left limit for the voting grid, in meters.
    - `x_max` the right limit for the voting grid, in meters.
    - `y_min` the bottom limit for the voting grid in meters.
    - `y_max` the top limit for the voting grid in meters.
    - `vote_collect_radius` the radius use during the collection of votes assigned
      to each detection.

    Returns a list of tuples (x,y,probs) where `probs` has the same layout as
    `probas`.
    '''
    if class_weights is not None:
        probas = array(probas)  # Make a copy.
        probas[:, :, 1:] *= class_weights
    vote_combiner = _agnostic_weighted_vote_avg if weighted_avg is True else _vote_avg
    x_range = int((x_max-x_min)/bin_size)
    y_range = int((y_max-y_min)/bin_size)
    grid = zeros((x_range, y_range, probas.shape[2]), float32)

    vote_collect_radius_sq = vote_collect_radius * vote_collect_radius

    # Update x/y max to correspond to the end of the last bin.
    x_max = x_min + x_range*bin_size
    y_max = y_min + y_range*bin_size

    # Where we collect the outputs.
    all_dets = []
    all_grids = []

    # Iterate over the scans. TODO: We can do most of this outside the looping too, actually.
    for iscan, (x, y, probs) in enumerate(zip(xs, ys, probas)):
        # Clear the grid, for each scan its own.
        grid.fill(0)
        all_dets.append([])

        # Filter out all the super-weak votes, as they wouldn't contribute much anyways
        # but waste time.
        voters_idxs = where(sum(probs[:,1:], axis=-1) > min_thresh)[0]

        # No voters, early bail
        if not len(voters_idxs):
            if retgrid:
                all_grids.append(array(grid))  # Be sure to make a copy.
            continue

        x = x[voters_idxs]
        y = y[voters_idxs]
        probs = probs[voters_idxs]

        # Convert x/y to grid-cells.
        x_idx: NDArray[int64] = int64((x-x_min)/bin_size)  # type: ignore
        y_idx: NDArray[int64] = int64((y-y_min)/bin_size)  # type: ignore

        # Discard data outside of the window.
        mask = (0 <= x_idx) & (x_idx < x_range) & (0 <= y_idx) & (y_idx < y_range)
        x_idx = x_idx[mask]
        x = x[mask]
        y_idx = y_idx[mask]
        y = y[mask]
        probs = probs[mask]

        # Vote into the grid, including the agnostic vote as sum of class-votes!
        #TODO Do we need the class grids?
        b_array = concatenate([sum(probs[:,1:], axis=-1, keepdims=True), probs[:,1:]], axis=-1)
        add.at(grid, (x_idx, y_idx), b_array)

        # Find the maxima (NMS) only in the "common" voting grid.
        grid_all = grid[:,:,0]
        if blur_win is not None and blur_sigma is not None:
            grid_all = GaussianBlur(grid_all, (blur_win,blur_win), blur_sigma)
        max_grid = maximum_filter(grid_all, size=3)
        maxima = (grid_all == max_grid) & (grid_all > 0)
        m_x, m_y = where(maxima)

        if len(m_x) == 0:
            if retgrid:
                all_grids.append(array(grid))  # Be sure to make a copy.
            continue

        # Back from grid-bins to real-world locations.
        m_x = m_x*bin_size + x_min + bin_size/2
        m_y = m_y*bin_size + y_min + bin_size/2

        # For each vote, get which maximum/detection it contributed to.
        # Shape of `center_dist` (ndets, voters) and outer is (voters)
        center_dist = square(x - m_x[:,None]) + square(y - m_y[:,None])
        det_voters = argmin(center_dist, axis=0)

        # Generate the final detections by average over their voters.
        for ipeak in range(len(m_x)):
            my_voter_idxs = where(det_voters == ipeak)[0]
            my_voter_idxs = my_voter_idxs[center_dist[ipeak, my_voter_idxs] < vote_collect_radius_sq]
            all_dets[-1].append(vote_combiner(x[my_voter_idxs], y[my_voter_idxs], probs[my_voter_idxs,:]))

        if retgrid:
            all_grids.append(array(grid))  # Be sure to make a copy.

    if retgrid:
        return all_dets, all_grids
    return all_dets


def _process_detections(det_x, det_y, det_p, det_f, wcs, was, wps, eval_r):
    allgts = [wc+wa+wp for wc, wa, wp in zip(wcs, was, wps)]
    gts_x, gts_y, gts_r, gts_f = _deep2flat_gt(allgts, radius=eval_r)
    wd_r, wd_p, wd_t = _prec_rec_2d(sum(det_p[:,1:], axis=1), c_[det_x, det_y], det_f, c_[gts_x, gts_y], gts_f, gts_r)
    gts_x, gts_y, gts_r, gts_f = _deep2flat_gt(wcs, radius=eval_r)
    # TODO possibly speed up the below significantly since a lot of them have 0 probability by design in some cases and can be dropped.
    wc_r, wc_p, wc_t = _prec_rec_2d(det_p[:,1], c_[det_x, det_y], det_f, c_[gts_x, gts_y], gts_f, gts_r)
    gts_x, gts_y, gts_r, gts_f = _deep2flat_gt(was, radius=eval_r)
    wa_r, wa_p, wa_t = _prec_rec_2d(det_p[:,2], c_[det_x, det_y], det_f, c_[gts_x, gts_y], gts_f, gts_r)
    gts_x, gts_y, gts_r, gts_f = _deep2flat_gt(wps, radius=eval_r)
    wp_r, wp_p, wp_t = _prec_rec_2d(det_p[:,3], c_[det_x, det_y], det_f, c_[gts_x, gts_y], gts_f, gts_r)

    return [wd_r, wd_p, wd_t], [wc_r, wc_p, wc_t], [wa_r, wa_p, wa_t], [wp_r, wp_p, wp_t]


def comp_prec_rec_softmax(scans, wcs, was, wps, pred_conf, pred_offs, eval_r=0.5, **v2d_kw):
    x, y = prepare_prec_rec_softmax(scans, pred_offs)
    detections = votes_to_detections(x, y, pred_conf, **v2d_kw)
    det_x, det_y, det_p, det_f = _deep2flat(detections)
    return _process_detections(det_x, det_y, det_p, det_f, wcs, was, wps, eval_r)


def calc_prec_rec_softmax(wcs, was, wps, predictions, eval_r=0.5):
    std_prob = array([0, 0, 0, 1], dtype=float32)
    deep = [[(p[:, 0][i], p[:, 1][i], std_prob.copy()) for i in range(len(p))] for p in predictions]
    det_x, det_y, det_p, det_f = _deep2flat(deep)
    return _process_detections(det_x, det_y, det_p, det_f, wcs, was, wps, eval_r)
