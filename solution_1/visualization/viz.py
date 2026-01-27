#!/usr/bin/env python3
"""
Generate PNG frames from heat2d CSV snapshots.
"""

import glob
import os
import numpy as np
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load_snapshot(path):
    x, y, u = np.loadtxt(path, delimiter=",", skiprows=1).T
    x = np.unique(x)
    y = np.unique(y)
    U = u.reshape((x.size, y.size))
    return x, y, U

def save_frames():

    paths = sorted(glob.glob("u_*.csv"))
    if not paths:
        print("No CSV snapshots found.", file=sys.stderr)
        return

    outdir = "frames"
    os.makedirs(outdir, exist_ok=True)

    for path in paths:
        step = os.path.splitext(path)[0]
        x, y, U = load_snapshot(path)

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
        ax.set_title(step)

        fig.savefig(os.path.join(outdir, f"{step}.png"), dpi=150, bbox_inches="tight")
        plt.close(fig)

if __name__ == "__main__":
    save_frames()

