import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

from fd import f2D, f3D
import mpl_config


def make_fig():
    x = np.linspace(0.0, np.pi, 200)[10:]

    fig, ax = plt.subplots()
    ax.set_xlim(x[0], x[-1])
    ax2 = ax.twinx()

    xticks = np.array([0.25, 0.5, 0.75, 1.0]) * np.pi
    xticks_labels = [f"${x}\,\pi$" for x in ["0.25", "0.5", "0.75", ""]]
    ax.set_xticks(ticks=xticks, labels=xticks_labels)

    ax.set_xlabel(r"$\sigma_i$")
    ax.set_ylabel(r"$f_{2\mathrm{D}}(\sigma_i)$")
    ax2.set_ylabel(r"$f_{3\mathrm{D}}(\sigma_i)$")

    ax.plot(x, f2D(x), color="C0")
    ax2.plot(x, f3D(x, n=100), color="C1", linestyle="--")

    ax.xaxis.set_minor_locator(mpl.ticker.AutoMinorLocator())

    ax.legend(
        [
            mpl.lines.Line2D([0], [0], color="C0"),
            mpl.lines.Line2D([0], [0], color="C1", linestyle="--"),
        ],
        [r"$2\mathrm{D}$", r"$3\mathrm{D}$"],
        loc="lower right",
    )

    mpl_config.save_fig(fig, "fd.png")


if __name__ == "__main__":
    mpl_config.configure()
    make_fig()
