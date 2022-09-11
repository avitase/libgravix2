import numpy as np

from src import gravix2


def test_lat(libgravix2, random):
    grvx = gravix2.load_library(libgravix2)

    z = 0.3
    lat = grvx.get_lat(z=z, fwd=False)
    assert np.allclose(lat, np.arcsin(z))
    assert np.allclose(lat, grvx.get_lat(z=z, fwd=True))

    m, n = 4, 100
    z = np.random.rand(m, n)

    lat = grvx.get_lat(z=z[0])
    assert lat.shape == (n,)
    for i in range(n):
        assert np.allclose(lat[i], grvx.get_lat(z=z[0, i]))

    lat = grvx.get_lat(z=z)
    assert lat.shape == (m, n)
    for i in range(m):
        for j in range(n):
            assert np.allclose(lat[i, j], grvx.get_lat(z=z[i, j]))


def test_lon(random, libgravix2):
    grvx = gravix2.load_library(libgravix2)

    x, y = 0.3, 0.4
    lon = grvx.get_lon(x=x, y=y, fwd=False)
    assert np.allclose(lon, np.arctan2(x, y))
    assert np.allclose(lon, grvx.get_lon(x=x, y=y, fwd=True))

    m, n = 4, 100
    x = np.random.rand(m, n)
    y = np.sqrt(1.0 - x ** 2)

    lon = grvx.get_lon(x=x[0], y=y[0])
    assert lon.shape == (n,)
    for i in range(n):
        assert np.allclose(lon[i], grvx.get_lon(x=x[0, i], y=y[0, i]))

    lon = grvx.get_lon(x=x, y=y)
    assert lon.shape == (m, n)
    for i in range(m):
        for j in range(n):
            assert np.allclose(lon[i, j], grvx.get_lon(x=x[i, j], y=y[i, j]))


def test_vlat(libgravix2, random):
    grvx = gravix2.load_library(libgravix2)

    x, y = 0.1, 0.2
    z = np.sqrt(1.0 - x ** 2 - y ** 2)

    vx, vy = 0.3, 0.4
    vz = -(x * vx + y * vy) / z

    lat, lon = grvx.get_lat(z=z), grvx.get_lon(x=x, y=y)

    vlat = grvx.get_vlat((vx, vy, vz), lat=lat, lon=lon, fwd=False)
    vlat_exp = (
        -vx * np.sin(lat) * np.sin(lon)
        - vy * np.sin(lat) * np.cos(lon)
        + vz * np.cos(lat)
    )
    assert np.allclose(vlat, vlat_exp)
    assert np.allclose(vlat, grvx.get_vlat((vx, vy, vz), lat=lat, lon=lon, fwd=True))

    m, n = 4, 100
    x = np.random.rand(m, n) * 0.5
    y = np.random.rand(m, n) * 0.5
    z = np.sqrt(1.0 - x ** 2 - y ** 2)

    vx = np.random.rand(m, n)
    vy = np.random.rand(m, n)
    vz = -(x * vx + y * vy) / z

    lat, lon = grvx.get_lat(z=z), grvx.get_lon(x=x, y=y)

    vlat = grvx.get_vlat((vx[0], vy[0], vz[0]), lat=lat[0], lon=lon[0])
    assert vlat.shape == (n,)
    for i in range(n):
        vlat_exp = grvx.get_vlat(
            (vx[0, i], vy[0, i], vz[0, i]), lat=lat[0, i], lon=lon[0, i]
        )
        assert np.allclose(vlat[i], vlat_exp)

    vlat = grvx.get_vlat((vx, vy, vz), lat=lat, lon=lon)
    assert vlat.shape == (m, n)
    for i in range(m):
        for j in range(n):
            vlat_exp = grvx.get_vlat(
                (vx[i, j], vy[i, j], vz[i, j]), lat=lat[i, j], lon=lon[i, j]
            )
            assert np.allclose(vlat[i, j], vlat_exp)


def test_vlon(libgravix2, random):
    grvx = gravix2.load_library(libgravix2)

    x, y = 0.1, 0.2
    z = np.sqrt(1.0 - x ** 2 - y ** 2)

    vx, vy = 0.3, 0.4
    vz = -(x * vx + y * vy) / z

    lon = grvx.get_lon(x=x, y=y)
    vlon = grvx.get_vlon((vx, vy, vz), lon=lon, fwd=False)
    vlon_exp = vx * np.cos(lon) - vy * np.sin(lon)
    assert np.allclose(vlon, grvx.get_vlon((vx, vy, vz), lon=lon, fwd=True))

    m, n = 4, 100
    x = np.random.rand(m, n) * 0.5
    y = np.random.rand(m, n) * 0.5
    z = np.sqrt(1.0 - x ** 2 - y ** 2)

    vx = np.random.rand(m, n)
    vy = np.random.rand(m, n)
    vz = -(x * vx + y * vy) / z

    lon = grvx.get_lon(x=x, y=y)

    vlon = grvx.get_vlon((vx[0], vy[0], vz[0]), lon=lon[0])
    assert vlon.shape == (n,)
    for i in range(n):
        vlon_exp = grvx.get_vlon((vx[0, i], vy[0, i], vz[0, i]), lon=lon[0, i])
        assert np.allclose(vlon[i], vlon_exp)

    vlon = grvx.get_vlon((vx, vy, vz), lon=lon)
    assert vlon.shape == (m, n)
    for i in range(m):
        for j in range(n):
            vlon_exp = grvx.get_vlon((vx[i, j], vy[i, j], vz[i, j]), lon=lon[i, j])
            assert np.allclose(vlon[i, j], vlon_exp)
