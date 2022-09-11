import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

from fd import f3D
import mpl_config


def df2D_over_sin(x):
    return 1.0 / (1.0 - np.cos(x)) ** 2


def df3D_over_sin(x, *, n, h):
    x = np.clip(x, a_min=None, a_max=np.pi - 1e-4)

    df = (f3D(x + h, n=n) - f3D(x - h, n=n)) / (2.0 * h)
    return df / np.sin(x)


def make_fig():
    x = np.linspace(0.0, np.pi, 200)[20:]

    fig, ax = plt.subplots()
    ax.set_xlim(x[0], x[-1])
    ax2 = ax.twinx()

    xticks = np.array([0.25, 0.5, 0.75, 1.0]) * np.pi
    xticks_labels = [f"${x}\,\pi$" for x in ["0.25", "0.5", "0.75", ""]]
    ax.set_xticks(ticks=xticks, labels=xticks_labels)

    ax.set_xlabel(r"$\sigma_i$")
    ax.set_ylabel(r"$f'_{2\mathrm{D}}(\sigma_i) / \sin \sigma_i$")
    ax2.set_ylabel(r"$f'_{3\mathrm{D}}(\sigma_i) / \sin \sigma_i$")

    ax.plot(x, df2D_over_sin(x), color="C0", label=r"$d=2$")
    ax2.plot(
        x, df3D_over_sin(x, n=100, h=1e-3), color="C1", label=r"$d=3$", linestyle="--"
    )

    ax.xaxis.set_minor_locator(mpl.ticker.AutoMinorLocator())

    ax.legend(
        [
            mpl.lines.Line2D([0], [0], color="C0"),
            mpl.lines.Line2D([0], [0], color="C1", linestyle="--"),
        ],
        [r"$2\mathrm{D}$", r"$3\mathrm{D}$"],
        loc="upper right",
    )

    mpl_config.save_fig(fig, "dfd_over_sin.png")


if __name__ == "__main__":
    mpl_config.configure()
    make_fig()
