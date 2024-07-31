"""
This file is totally based on research papaer about DROW.
Here the link to the paper can be found:
https://arxiv.org/abs/1603.02636
The utility functions present here can be found in paper's GitHub:
https://github.com/VisualComputingInstitute/DROW/blob/master/v2/utils/__init__.py
"""

from numpy import max, clip, where, argmin, abs

from sklearn.metrics import auc

from matplotlib.pyplot import legend, subplots
from matplotlib.ticker import FuncFormatter


def _prettify_pr_curve(ax):
    ax.plot([0,1], [0,1], ls="--", c=".6")
    ax.set_xlim(-0.02,1.02)
    ax.set_ylim(-0.02,1.02)
    ax.set_xlabel("Recall [%]")
    ax.set_ylabel("Precision [%]")
    ax.axes.xaxis.set_major_formatter(FuncFormatter(lambda x, _: '{:.0f}'.format(x*100)))
    ax.axes.yaxis.set_major_formatter(FuncFormatter(lambda x, _: '{:.0f}'.format(x*100)))
    return ax


def _fatlegend(ax=None, *args, **kwargs):
    """
    This function is totally based on Lucas Eyer's toolbox library.
    It is named `lbtoolbox`. Please, admire it with respect:
    https://github.com/lucasb-eyer/lbtoolbox
    Original name: `fatlegend`
    """
    if ax is not None:
        leg = ax.legend(*args, **kwargs)
    else:
        leg = legend(*args, **kwargs)

    for l in leg.legendHandles:  # type: ignore
        l.set_linewidth(l.get_linewidth()*2.0)
        l.set_alpha(1)
    return leg



def _peakf1(recs, precs):
    return max(2*precs*recs/clip(precs+recs, 1e-16, 2+1e-16))


def _eer(recs, precs):
    # Find the first nonzero or else (0,0) will be the EER :)
    p1 = where(precs != 0)[0][0]
    r1 = where(recs != 0)[0][0]
    idx = argmin(abs(precs[p1:] - recs[r1:]))
    return (precs[p1+idx] + recs[r1+idx])/2  # They are often the exact same, but if not, use average.


def plot_prec_rec(wds, wcs, was, wps, figsize=(15,10), title=None):
    fig, ax = subplots(figsize=figsize)

    ax.plot(*wds[:2], label='agn (AUC: {:.1%}, F1: {:.1%}, EER: {:.1%})'.format(auc(*wds[:2]), _peakf1(*wds[:2]), _eer(*wds[:2])), c='#E24A33')
    ax.plot(*wcs[:2], label='wcs (AUC: {:.1%}, F1: {:.1%}, EER: {:.1%})'.format(auc(*wcs[:2]), _peakf1(*wcs[:2]), _eer(*wcs[:2])), c='#348ABD')
    ax.plot(*was[:2], label='was (AUC: {:.1%}, F1: {:.1%}, EER: {:.1%})'.format(auc(*was[:2]), _peakf1(*was[:2]), _eer(*was[:2])), c='#988ED5')
    ax.plot(*wps[:2], label='wps (AUC: {:.1%}, F1: {:.1%}, EER: {:.1%})'.format(auc(*wps[:2]), _peakf1(*wps[:2]), _eer(*wps[:2])), c='#8EBA42')

    if title is not None:
        fig.suptitle(title, fontsize=16, y=0.91)

    _prettify_pr_curve(ax)
    _fatlegend(ax, loc='upper right')
    return fig, ax
