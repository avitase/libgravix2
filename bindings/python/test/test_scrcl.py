import numpy as np

from src import gravix2


def shoot(missile, *, planets, n=1, h=1e-3):
    x = np.empty((0, 3))
    v = np.empty((0, 3))

    premature = False
    while n > 0 and not premature:
        n -= 1
        premature = missile.propagate(planets, h=h)

        x = np.concatenate((x, missile.trajectory.x), axis=0)
        v = np.concatenate((v, missile.trajectory.v), axis=0)

    return x, v, premature


def test_missiles(libgravix2):
    grvx = gravix2.load_library(libgravix2)

    r0 = 0.2
    v0 = grvx.v_scrcl(r0)

    planets = grvx.new_planets([(0.0, 0.0)])

    missiles = grvx.new_missiles(1)
    missiles[0].set(pos=(r0, 0.0), orientation=(0.0, 1.0), v=v0)

    x, v, premature = shoot(missiles[0], planets=planets, n=20)
    assert not premature

    lat = np.arcsin(x[:, 2])
    lon = np.arctan2(x[:, 0], x[:, 1])
    dist = np.arccos(np.cos(lat) * np.cos(lon))
    v_abs = np.sqrt(np.sum(v**2, axis=1))

    r_error = np.rad2deg(dist - r0)
    v_error = np.rad2deg(v_abs - v0)

    assert np.max(np.abs(r_error)) < 1e-11
    assert np.max(np.abs(v_error)) < 1e-11
