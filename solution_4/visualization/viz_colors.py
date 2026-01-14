#!/usr/bin/env python3
"""
Generate PNG frames from multi-rank heat2d CSV snapshots.
"""

import glob
import os
import re
import numpy as np

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


_iter_re = re.compile(r"u_(\d{6})_(\d+)\.csv$")


def _group_by_iteration(paths):
    groups = {}
    for p in paths:
        m = _iter_re.search(os.path.basename(p))
        if not m:
            continue
        it = m.group(1)
        groups.setdefault(it, []).append(p)
    return [(it, sorted(ps)) for it, ps in sorted(groups.items())]


def _assemble_grid_from_paths(paths):
    rows_list = []
    for p in paths:
        arr = np.loadtxt(p, delimiter=",", skiprows=1)
        if arr.ndim == 1:
            arr = arr.reshape(1, -1)
        rows_list.append(arr[:, :3])   # x, y, u

    rows = np.vstack(rows_list)

    x_unique = np.unique(rows[:, 0])
    y_unique = np.unique(rows[:, 1])

    U = np.full((y_unique.size, x_unique.size), np.nan)

    xi = {v: i for i, v in enumerate(x_unique)}
    yi = {v: i for i, v in enumerate(y_unique)}

    for xv, yv, uv in rows:
        U[yi[yv], xi[xv]] = uv

    if np.isnan(U).any():
        print(f"Warning: {np.isnan(U).sum()} missing cells", file=sys.stderr)

    return x_unique, y_unique, U


def save_frames_multi():
    groups = _group_by_iteration(sorted(glob.glob("u_*.csv")))
    if not groups:
        print("No multi-rank snapshots found.", file=sys.stderr)
        return

    os.makedirs("frames_multi", exist_ok=True)

    # get grid geometry from the first iteration only
    x, y, U = _assemble_grid_from_paths(groups[0][1])

    fig, ax = plt.subplots()
    im = ax.imshow(
        U,
        origin="lower",
        extent=[x[0], x[-1], y[0], y[-1]],
        aspect="equal",
        vmin=0,
        vmax=1,
    )
    fig.colorbar(im, ax=ax).set_label("Temperature")
    ax.set_xlabel("x")
    ax.set_ylabel("y")

    # unified loop over all iterations, including the first one
    for it, ps in groups:
        _, _, U = _assemble_grid_from_paths(ps)
        im.set_data(U)
        ax.set_title(f"u_{it} (ranks: {len(ps)})")
        fig.savefig(f"frames_multi/u_{it}.png", dpi=150, bbox_inches="tight")
        fig.clf()

    plt.close(fig)


if __name__ == "__main__":
    save_frames_multi()
