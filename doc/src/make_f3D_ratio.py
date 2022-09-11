import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

from fd import f3D
import mpl_config


def make_fig():
    x = np.linspace(0.0, np.pi, 200)[10:]

    fig, ax = plt.subplots()

    ax.set_xlim(x[0], x[-1])
    ax.set_ylim(1.0, 1.055)

    xticks = np.array([0.25, 0.5, 0.75, 1.0]) * np.pi
    xticks_labels = [f"${x}\,\pi$" for x in ["0.25", "0.5", "0.75", ""]]
    ax.set_xticks(ticks=xticks, labels=xticks_labels)

    ax.set_xlabel(r"$\sigma_i$")
    ax.set_ylabel(
        r"$\left. f^{(J)}_{3\mathrm{D}}(\sigma_i) \;\middle/\; f^{(0)}_{3\mathrm{D}}(\sigma_i) \right.$"
    )

    y0 = f3D(x, n=1)

    for n in [2, 3, 8, 101]:
        y = f3D(x, n=n) / y0
        ax.plot(x, y, label=f"$J={n-1}$", zorder=-n, alpha=0.8)

    ax.legend()
    ax.grid()

    ax.xaxis.set_minor_locator(mpl.ticker.AutoMinorLocator())
    ax.yaxis.set_minor_locator(mpl.ticker.AutoMinorLocator())

    mpl_config.save_fig(fig, "f3D_ratio.png")


if __name__ == "__main__":
    mpl_config.configure()
    make_fig()
