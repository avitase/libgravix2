import numpy as np


def f2D(x):
    return -1.0 / (1.0 - np.cos(x))


def f3D(x, *, n):
    arg = x - np.pi

    y = 0
    for j in range(n):
        k = 2.0 * j + 1.0
        y += k / ((k * np.pi) ** 2 - arg ** 2) ** 2

    return -y / np.sinc(arg / np.pi)
