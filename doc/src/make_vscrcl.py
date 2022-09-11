import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

from fd import f3D
import mpl_config


def v_scrcl2D(r):
    v = np.sqrt((1.0 + np.cos(r)) / np.abs(np.cos(r)))
    return v


def v_scrcl3D(r, *, n):
    f = f3D(r, n=n)
    v = np.sin(r) * np.sqrt(-f / np.abs(np.cos(r)))
    return v


def make_fig(n=100):
    r1 = np.linspace(0, np.pi / 2.0, 200)[2:-1]
    r2 = np.linspace(np.pi / 2.0, np.pi, 200)[1:]
    v2D_r1 = v_scrcl2D(r1)
    v2D_r2 = v_scrcl2D(r2)
    v3D_r1 = v_scrcl3D(r1, n=n)
    v3D_r2 = v_scrcl3D(r2, n=n)

    fig, ax = plt.subplots()
    ax2 = ax.twinx()

    ax.set_xlim(r1[0], r2[-1])
    ax.set_ylim(0, max(v2D_r1[-1], v2D_r2[0]))
    ax2.set_ylim(0, max(v3D_r1[-1], v3D_r2[0]))

    xticks = np.array([0.25, 0.5, 0.75, 1.0]) * np.pi
    xticks_labels = [f"${x}\,\pi$" for x in ["0.25", "0.5", "0.75", ""]]
    ax.set_xticks(ticks=xticks, labels=xticks_labels)
    ax.xaxis.set_minor_locator(mpl.ticker.AutoMinorLocator())

    ax.axvline(np.pi / 2.0, color="k", linestyle=":", linewidth=1)

    ax.plot(r1, v2D_r1, c="C0")
    ax.plot(r2, v2D_r2, c="C0")
    ax2.plot(r1, v3D_r1, c="C1", linestyle="--")
    ax2.plot(r2, v3D_r2, c="C1", linestyle="--")

    ax.set_xlabel("Distance to planet")
    ax.set_ylabel("$v_{\mathrm{orb},2\mathrm{D}}$")
    ax2.set_ylabel("$v_{\mathrm{orb},3\mathrm{D}}$")

    ax.legend(
        [
            mpl.lines.Line2D([0], [0], color="C0"),
            mpl.lines.Line2D([0], [0], color="C1", linestyle="--"),
        ],
        [
            r"2D",
            r"3D",
        ],
    )

    mpl_config.save_fig(fig, "vscrcl.png")


if __name__ == "__main__":
    mpl_config.configure()
    make_fig()
