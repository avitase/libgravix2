import matplotlib.pyplot as plt
import numpy as np

import mpl_config 


def eval_pot(x, *, j, threshold=5):
    y0 = -2.0 / (np.pi * (2.0 * j + 1))
    y = -1.0 / (2.0 * np.pi * j + x) - 1.0 / (2.0 * np.pi * (j + 1) - x) - y0

    xs = []
    ys = []

    i_first = 0
    seq_done = False
    for i in range(len(x)):
        if abs(y[i]) > threshold:
            if not seq_done:
                seq_done = True
                xs.append(x[i_first:i])
                ys.append(y[i_first:i])
        elif seq_done:
            seq_done = False
            i_first = i

    xs.append(x[i_first:])
    ys.append(y[i_first:])

    return xs, ys


def make_fig(n=3):
    x = np.linspace(-14, 20.5, 800)
    seqs = [eval_pot(x, j=j) for j in range(n)]

    fig, ax = plt.subplots()

    xticks = np.array([-4, -2, 0, 2, 4, 6]) * np.pi
    xticks_labels = [
        f"${x}$" for x in [r"-4\pi", r"-2\pi", r"0", r"2\pi", r"4\pi", r"6\pi"]
    ]
    ax.set_xticks(ticks=xticks, labels=xticks_labels)

    ax.set_xlabel(r"$\sigma_i$")
    ax.set_ylabel(r"$V_{\!3\mathrm{D}}^{(j)}(\sigma_i)$")
    ax.set_xlim(x[0], x[-1])
    ax.set_ylim(-4, 4)
    ax.grid()

    for j, seq in enumerate(seqs):
        label = f"$j={j}$"

        for x0, y0 in zip(*seq):
            s1 = x0 > 0
            s2 = x0 < 2.0 * np.pi
            ax.plot(x0[s1 & s2], y0[s1 & s2], color=f"C{j}", label=label)

            sel = x0 < 0
            ax.plot(x0[~s1], y0[~s1], "--", color=f"C{j}")

            sel = x0 > 2.0 * np.pi
            ax.plot(x0[~s2], y0[~s2], "--", color=f"C{j}")
            label = None

    ax.legend(loc="upper center")

    mpl_config.save_fig(fig, "pot3D.png")


if __name__ == "__main__":
    mpl_config.configure()
    make_fig()
