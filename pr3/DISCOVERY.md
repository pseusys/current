# People Detection Using 2D Lidar: Research and Architecture

---

## Problem Statement

A mobile robot carries a 2D lidar scanner mounted at leg height (~40 cm off the ground). The scanner rotates in a horizontal plane and produces a single array of **N range measurements** per timestep — one distance value `r_i` for each beam angle `φ_i`, uniformly distributed across the sensor's field of view (DROW: N ≈ 450 beams at 0.5° spacing; FROG: N = 720 beams at 0.5° spacing).

The task is **beam-level classification with spatial vote regression**: for every beam `i` in every scan, predict:
1. A probability over four classes: background, wheelchair (wc), walker (wa), person/pedestrian (wp)
2. A 2D vote offset `(dx, dy)` in the sensor frame pointing toward the nearest annotation centre

Post-processing clusters the vote targets from all beams that voted "positive" into discrete person/wheelchair/walker detections (Gaussian-smoothed accumulator + peak detection), avoiding the need to predict the exact count of people.

### Key challenges

| Challenge | Description |
|---|---|
| **Scale variance** | A person at 2 m subtends ~12 beams; the same person at 8 m subtends ~3 beams |
| **Sparse targets** | People occupy <5 % of beams in a typical indoor scan |
| **Angular ambiguity** | The sensor measures range, not shape; a thin pole and a leg look similar locally |
| **Temporal drift** | The robot moves between scans, so consecutive scans are in different frames |
| **Class imbalance** | Background beams outnumber positive beams by ~20:1 |

### Evaluation metric

The standard metric is **Average Precision (AP)** at a matching radius of 0.5 m in Cartesian space, computed separately per class. Numbers throughout this document use the DROW test set as the reference benchmark.

---

## Previous Research

### DROW — Beyer et al. (ICRA 2016, IROS 2018)

**Reference:** arXiv:1603.02636

#### Key idea: the cutout

For each beam `i`, a window of `S = 48` neighbouring beam ranges is extracted in polar coordinates and centred on beam `i`. This **cutout** forms a 1D signal of length 48 that encodes the local angular neighbourhood of the beam.

A shared 1D CNN is applied to every cutout independently — beams are processed in parallel with no cross-beam communication during feature extraction. The network has three convolutional blocks followed by two upsampling heads (class logits and vote offset).

#### Temporal aggregation: fixed sum

When using `T` consecutive scans, the per-beam CNN is applied to each timestep independently, producing `T` feature vectors per beam. These are simply **summed** across the time dimension before the detection heads. All timesteps are weighted equally; the network has no way to down-weight a noisy or motion-blurred frame.

```
for each timestep t = 1…T:
    cutout_t[i]  = extract_48_beams(scan_t, beam_i)
    feature_t[i] = CNN(cutout_t[i])          ← no cross-beam communication

feature[i] = Σ_t feature_t[i]               ← fixed sum, equal weights
logit[i], vote[i] = detection_head(feature[i])
```

#### Odometry alignment

Before feature extraction, historical scans are rotated into the current scan's coordinate frame using the robot's odometry (differential rotation Δθ between timesteps). Only rotation is applied — translation is ignored, approximating that the robot has moved little between T consecutive frames.

#### Performance

| Class | AP (DROW test set) |
|---|---|
| Person (wp) | 0.619 |
| Wheelchair (wc) | 0.658 |
| Walker (wa) | 0.520 |

#### Identified weaknesses

1. **Zero cross-beam communication**: the CNN processes each 48-beam window in complete isolation. Two adjacent beams, each individually ambiguous, cannot reinforce each other's detection.
2. **Fixed temporal weighting**: a scan frame where the person was occluded or the sensor returned a spurious reading contributes equally to the sum.
3. **Fixed receptive field**: the 48-beam cutout is hardcoded; the network cannot look further or closer based on what it sees.
4. **Polar-only representation**: the cutout contains raw range values. The CNN must learn to be invariant to the physical distance of the person, which changes the angular width of the cluster.

#### ⚠ Annotation quality concern

The DROW dataset uses **sparse annotations**: only a subset of scan frames in each recording sequence carries ground truth labels. Frames between annotated keyframes have no labels at all — they are neither positive nor explicitly marked as negative.

This creates two training pathologies:

- **False-negative label noise.** If a person is present in an unannotated frame, any detection there is scored as a false positive during training. The model is penalised for correct detections.
- **Coverage gaps.** The fraction of annotated frames is low (DROW train: 17 665 annotated out of far more recorded frames), so the model sees far fewer positive examples per sequence than actually exist in the data.

Additionally, ground truth positions were obtained by correlating lidar timestamps with an external reference (camera footage or motion capture), introducing **temporal synchronisation error** at fast motion speeds. Annotations very close to the sensor (< 1 m) or at the edge of the field of view are known to be less reliable due to projection ambiguity.

The FROG dataset was recorded specifically to address these issues: every scan frame is annotated, eliminating false-negative noise and providing a denser training signal. When training on DROW and evaluating metrics, these annotation limitations should be kept in mind when interpreting low recall figures.

---

### DR-SPAAM — Jia et al. (IROS 2020, RA-L 2022)

**Reference:** arXiv:2004.14064

DR-SPAAM retains the DROW cutout extraction and per-beam CNN but replaces the fixed temporal sum with an **auto-regressive spatial attention mechanism** that directly addresses DROW's weaknesses (1) and (2).

Our implementation is the official **SpatialDROW** from the RA-L 2022 release, loaded from the published checkpoint `dr_spaam_e40.pth`.

#### Architecture: four conv blocks + spatial attention gate

The per-beam CNN is extended from DROW's two blocks to four, using the same `_conv3x3` (Conv1d + BN + LeakyReLU) building block:

```
Block 1  (1→128,  3 layers) + MaxPool(2)  ↘
Block 2  (128→256, 3 layers) + MaxPool(2)  → 256-ch feature map per beam (14 pts for S=56)
                                             ↓ spatial attention gate
Block 3  (256→512, 3 layers) + MaxPool(2)
Block 4  (512→128, 2 layers) + AvgPool
Conv1d heads → logits (1 or 4 classes), votes (2)
```

#### Auto-regressive spatial attention (the "A" in DR-SPAAM)

After blocks 1–2, each beam has a feature map `f_t[i]` (spatial, not pooled). The temporal aggregation uses an **auto-regressive template** rather than explicit temporal attention weights:

```
template_0 = f_0[:]              ← initialised from first scan in window (detached)

for t = 1…T−1:
    feat_t[i] = encode(scan_t, beam_i)   ← blocks 1–2

    # spatial attention: beam i attends to ±window/2 neighbours in template
    attn_weight[i,j] = softmax( embed(feat_t[i]) · embed(template[j])
                                for j in i−K … i+K )
    template[i] = α · feat_t[i]  +  (1−α) · Σ_j attn_weight[i,j] · template[j]
                  ↑ current feat       ↑ attended historical template

final_feature[:] = decode(template[:])   ← blocks 3–4 + heads
```

Key properties:

- **Stop-gradient on template input**: gradients only flow through the current scan's encode/gate path, not back through the full T-step chain (prevents BPTT instability).
- **Local angular context** (±5 neighbours, `window_size=11`): beam `i` can observe its closest angular neighbours in the historical template.
- **Alpha blending** (`alpha=0.5`): the gate blends current features with the attended template, preventing the template from collapsing to a running mean.
- **56-pt cutouts** (`N_SAMP=56`): published weights use a 56-sample polar window instead of DROW's 48.
- **Pedestrian-only output**: published weights (`dr_spaam_e40.pth`) output a single sigmoid class (person only). Our training configuration uses 4 classes.

#### Ablation results from the paper

| Component | AP (wp) | Δ vs DROW |
|---|---|---|
| DROW (sum only) | 0.619 | — |
| Temporal attn only | ~0.640 | +2.1 pp |
| Spatial attn only | ~0.659 | +4.0 pp |
| DR-SPAAM (both) | **0.696** | **+7.7 pp** |
| DR-SPAAM RA-L (2022) | **0.720+** | **+10+ pp** |

#### Limitations of DR-SPAAM

1. **Still uses fixed 56-beam cutouts**: the CNN has no access to raw data beyond 56 beams, and the cutout boundary is hardcoded.
2. **Spatial attention operates on compressed features**: neighbouring beams only communicate *after* independent CNN processing. Raw-signal cross-beam context is impossible.
3. **Local attention only** (±5 neighbours): each beam can see only its closest neighbours. Global scan structure (e.g., two legs of the same person widely separated) is not captured.
4. **Performance-driven design**: DR-SPAAM was developed under real-time constraints (ROS node, embedded hardware). Many architectural choices reflect computational budget rather than theoretical optimality.

---

## Preprocessing

### Raw sensor data

Each scan is a 1D array of `N` range measurements:

```
scan = [r_0, r_1, …, r_{N-1}]    r_i ∈ [r_min, r_max] metres
```

The beam angles are fixed and sensor-specific:

```
φ_i = φ_min + i · Δφ             (uniformly spaced, e.g. Δφ = 0.5° = 0.00873 rad)
```

### Odometry alignment

When stacking `T` consecutive scans to form the temporal input, the robot has moved between frames. Only the **rotation component** of odometry (Δθ) is used — translation is negligible over T=5 frames at typical indoor robot speeds.

For historical scan at time `t − k`:
1. Compute the accumulated rotation `Δθ_k` from odometry between `t−k` and `t`
2. Shift each beam's angle: `φ'_i = φ_i + Δθ_k`
3. Re-sample onto the original beam grid via nearest-neighbour: `r'_j = r_i` where `i = round((φ'_j − φ_min) / Δφ)`

This brings all T scans into the coordinate frame of the current scan.

### Coordinate representation

#### The choice: `(r, x, y)` triplet

For each beam `i` in each aligned scan, compute:

```python
x_i = -r_i * sin(φ_i)    # lateral  (DROW frame: x = sideways)
y_i =  r_i * cos(φ_i)    # forward  (DROW frame: y = forward)
```

The input feature per beam per timestep is therefore a **3-channel vector** `(r_i, x_i, y_i)`.

For a full-scan network (see Architectures), the input is `(T, N_beams, 3)`.

#### Reasoning for this choice

**Why include `r` (polar range):**
A 1D CNN with fixed kernel size operates over a constant number of beams regardless of range. A person at 2 m spans ~12 beams; at 8 m, the same person spans ~3 beams. The raw range value `r_i` is the only signal that tells the network *how far away* this beam endpoint is — enabling the network to learn scale-aware features ("at this range, a cluster spanning K beams corresponds to ~0.5 m, which is person-sized").

**Why include `(x, y)` (Cartesian coordinates):**
A person's body always occupies a roughly constant-diameter region in Cartesian space (~0.5–0.8 m), regardless of which direction the robot faces. After odometry alignment, a person at position `(x_p, y_p)` generates a cluster of beam endpoints concentrated around that Cartesian point across all T timesteps. Providing `(x, y)` directly allows a 1D conv kernel to detect this Cartesian cluster — learning a filter that fires when adjacent beams have similar `(x, y)` values, which is equivalent to detecting a dense point cluster.

Without Cartesian coordinates, the network would have to compute them implicitly from `r` and the beam index, which requires the network to learn trigonometric relationships — an unnecessary burden.

**Why not `(x, y)` alone:**
`(x, y)` without `r` loses the explicit distance signal. The CNN cannot directly reason about scale: two beams with identical `(x, y)` values at very different ranges produce very different angular patterns, but the Cartesian representation hides this distinction.

**Why not first differences `(Δr, Δx, Δy)`:**
A 1D CNN with kernel `[-1, +1]` can compute first differences internally; providing them as input is redundant. Absolute values are numerically more stable and give the network maximum flexibility to learn what features matter.

**Redundancy:** `x² + y² = r²`, so the three values are not independent. This creates a null space in the first convolutional layer's weight matrix, but does not prevent learning — the network simply ignores the redundant directions in weight space.

#### Cutout extraction (current pipeline)

The cutout approach (used by DROW and DR-SPAAM) replaces the 3-channel full-scan input with a **48-sample polar window** extracted around each beam:

```
cutout[i] = [r_{i-24}, r_{i-23}, …, r_i, …, r_{i+23}]   ← raw range values only
```

This reduces the input from `(N_beams, 3)` to `(N_beams, 48)` at the cost of:
- Discarding all cross-beam information beyond 48 beams
- Using raw `r` values only (no `x, y`)
- Hardcoding the spatial context window

The cutout is kept in the current implementation for compatibility with the existing training pipeline and DrowDetector inference API. The full-scan `(r, x, y)` approach (see below) is the proposed direction for next-generation models.

---

## Architectures

### 1. Full-Scan 1D Dilated CNN (FullScanCNNDetector)

The core idea is to eliminate the cutout entirely and process the **full scan** as a 1D signal, using dilated convolutions to build up a multi-scale receptive field.

#### Input

```
(T, N_beams, 3)   ← stack of T odometry-aligned scans, each beam has (r, x, y)
```

#### Architecture

```
For each timestep t independently:
  Conv1d(3,    64,  kernel=3, dilation=1 ) → receptive field  3 beams
  Conv1d(64,  128,  kernel=3, dilation=2 ) → receptive field  7 beams
  Conv1d(128, 256,  kernel=3, dilation=4 ) → receptive field 15 beams
  Conv1d(256, 256,  kernel=3, dilation=8 ) → receptive field 31 beams
  Conv1d(256, 256,  kernel=3, dilation=16) → receptive field 63 beams
  → (N_beams, 256) per timestep

Temporal: GRU over T, per beam
  → (N_beams, 256)

Detection heads: Conv1d(256, 4) + Conv1d(256, 2)
  → (N_beams, 4), (N_beams, 2)
```

#### Advantages over cutout approaches

- No fixed receptive field — the network learns appropriate spatial context
- `(r, x, y)` input encodes both scale and geometry
- Dilated convs give exponentially growing context with linear parameter growth
- No preprocessing step (no cutout extraction per beam)

#### Disadvantage

Eliminates the simple per-beam parallelism of the cutout approach. The training loop must be redesigned to pass full scans and apply odometry alignment at the scan level.

---

### 2. 2D Space-Time CNN (SpaceTimeCNNDetector)

Stack all T aligned scans into a 2D array treating beam index as one axis and time as the other:

```
Input: (N_beams, T, 3)  ←  a "lidar video" with 3 channels

2D Conv(3,   64,  kernel=(5,3), padding=(2,1))   ← 5 beams × 3 frames
2D Conv(64,  128, kernel=(5,3), dilation=(2,1))  ← 9 beams × 3 frames
2D Conv(128, 256, kernel=(5,1))                  ← 9 beams, same frame only
Conv1d(256, 4) + Conv1d(256, 2)                  ← detection heads
```

A 2D kernel `(beam_width, time_width)` jointly encodes "what does a person look like in space-time?". A person creates a consistent angular stripe in the `(N_beams × T)` image. This is the most natural formulation but requires T to be treated as a spatial dimension of fixed size.

---

### 3. Full-Scan Dilated CNN + Beam Transformer (FullScanTransformerDetector)

Uses global multi-head self-attention over all N_beams simultaneously, after a dilated CNN backbone:

```
Full-scan dilated CNN backbone  → (N_beams, T, C)
BeamSelfAttention               → each beam attends to all N_beams (at each T)
Temporal GRU                    → (N_beams, C)
Detection heads
```

This is the most expressive beam-level architecture in the family: the Transformer gives content-adaptive, global beam-to-beam attention. The GRU provides sequential temporal reasoning.

**Cost:** The Transformer attention matrix is `N_beams × N_beams = 450 × 450 = 202,500` entries per layer. Feasible on GPU; ~1.5 s/scan on CPU.

---

### Architecture comparison

| Architecture | Beam communication | Temporal | Scale-aware input |
|---|---|---|---|
| DrowDetector | None | Fixed sum | No (polar only) |
| DrSpaamDetector | Local (±3 beams, auto-regressive) | Learned scalar | No |
| FullScanCNNDetector | Local → grows (dilated, ±15 beams) | GRU | **(r, x, y)** |
| SpaceTimeCNNDetector | Local (spatial + temporal jointly) | Implicit | **(r, x, y)** |
| FullScanTransformerDetector | **Global** | GRU | **(r, x, y)** |

All five detectors are implemented in `follow_the_drow.detectors` and trainable via `train.py`:

```bash
python train.py --detector drow|drspaam|fullscan_cnn|spacetime_cnn|fullscan_transformer
```
