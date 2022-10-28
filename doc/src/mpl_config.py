import matplotlib as mpl

from pathlib import Path


def configure():
    mpl.use("pgf")
    mpl.rcParams["font.family"] = "serif"
    mpl.rcParams["text.usetex"] = True
    mpl.rcParams["pgf.texsystem"] = "lualatex"
    mpl.rcParams["pgf.rcfonts"] = False
    mpl.rcParams["savefig.dpi"] = 400


def save_fig(fig, file_name):
    img_dir = Path(__file__).resolve().parent.parent / "img"

    fig.set_size_inches(4, 3)
    fig.tight_layout()
    fig.savefig(img_dir / file_name)
