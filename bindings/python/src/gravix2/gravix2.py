import ctypes
import functools
from pathlib import Path
from typing import Optional, Sequence, Tuple, Union

import numpy as np
from numpy.typing import ArrayLike

from . import config
from . import helper
from . import missile
from . import planet


class Gravix2:
    """
    Proxy class for ``libgravix2``

    :param lib: Library path to ``libgravix2.so``
    """

    def __init__(self, lib: Union[str, Path, ctypes.CDLL]) -> None:
        if type(lib) is ctypes.CDLL:
            self._lib = lib
        else:
            lib = Path(lib)
            if not lib.is_file():
                raise ValueError(f"Invalid library path {lib}")

            self._lib = ctypes.cdll.LoadLibrary(str(lib.resolve()))

        self._config = config.get_config(lib=self._lib)
        self._helper = helper.Helper(lib=self._lib)

    @property
    def config(self) -> config.Config:
        """
        Wraps ``libgravix2``'s ``GrvxConfig`` object

        :return: Static configuration as a :class:`gravix2.config.Config` instance
        """
        return self._config

    def new_planets(
        self,
        planets: Union[int, Sequence[Tuple[float, float]]],
        *,
        seed: Optional[int] = None,
        min_dist: Optional[float] = None,
    ) -> planet.Planets:
        """
        Creates a new set of planets

        Planets are either initialized explicitly or randomly via ``libgravix2``'s
        ``grvx_rnd_init_planets()`` (from the game extension.)

        If initialized randomly, a min. distance between planets has to be given.

        :param planets: Number of planets (random initialization) or list of latitude
                        and longitude pairs (explicit initialization.) If the parameter
                        is an integer, this many planets are initialized randomly.
                        Otherwise, the latitude/longitude pairs are used for (explicit)
                        initialization.
        :param seed: If planets are initialized randomly, a seed can be passed to make
                     the sampling result consistent. If set to ``None``, a random seed
                     will be generated internally.
        :param min_dist: If planets are initialized randomly, a min. distance is
                         required. See ``libgravix2``'s ``grvx_rnd_init_planets()`` for
                         details (from the game extensions.)
        :return: A new set of planets
        """
        return planet.Planets(planets, seed=seed, min_dist=min_dist, lib=self._lib)

    def new_missiles(self, missiles: int) -> missile.Missiles:
        """
        Creates a new set of uninitialized missiles

        :param missiles: Number of missiles
        :return: A new set of missiles
        """
        return missile.Missiles(int(missiles), lib=self._lib)

    def get_lat(
        self, *, z: Union[float, ArrayLike], fwd: bool = False
    ) -> Union[float, ArrayLike]:
        """
        Reimplements ``libgravix2``'s helper function ``grvx_lat()`` for NumPy arrays

        :param z: (Batch of) First parameter of ``grvx_lat()``
        :param fwd: Bypass reimplementation and forward call to ``libgravix2`` (does not
                    work for NumPy arrays)
        :return: (Batch of) Latitude
        """
        return self._helper.get_lat(float(z)) if fwd else np.arcsin(z)

    def get_lon(
        self,
        *,
        x: Union[float, ArrayLike],
        y: Union[float, ArrayLike],
        fwd: bool = False,
    ) -> Union[float, ArrayLike]:
        """
        Reimplements ``libgravix2``'s helper function ``grvx_lon()`` for NumPy arrays

        :param x: (Batch of) First parameter of ``grvx_lon()``
        :param y: (Batch of) Second parameter of ``grvx_lon()``
        :param fwd: Bypass reimplementation and forward call to ``libgravix2`` (does not
                    work for NumPy arrays)
        :return: (Batch of) Longitude
        """
        return self._helper.get_lon(float(x), float(y)) if fwd else np.arctan2(x, y)

    def get_vlat(
        self,
        v: Union[Tuple[float, float, float], ArrayLike],
        *,
        lat: Union[float, ArrayLike],
        lon: Union[float, ArrayLike],
        fwd: bool = False,
    ) -> Union[float, ArrayLike]:
        """
        Reimplements ``libgravix2``'s helper function ``grvx_v_lat()`` for NumPy arrays

        :param v: Tuple of first three parameters of ``grvx_v_lat()`` or NumPy array of
                  shape (\*, 3)
        :param lat: (Batch of) Fourth parameter of ``grvx_v_lat()``
        :param lon: (Batch of) Fifth parameter of ``grvx_v_lat()``
        :param fwd: Bypass reimplementation and forward call to ``libgravix2`` (does not
                    work for NumPy arrays)
        :return: (Batch of) Latitudinal speed
        """
        if fwd:
            vx, vy, vz = v
            return self._helper.get_vlat(
                float(vx), float(vy), float(vz), float(lat), float(lon)
            )

        v = np.array(v)
        if v.shape[0] != 3:
            raise ValueError("Last dimension of v has to be of size 3")

        vx = v[0]
        vy = v[1]
        vz = v[2]

        return (
            -vx * np.sin(lat) * np.sin(lon)
            - vy * np.sin(lat) * np.cos(lon)
            + vz * np.cos(lat)
        )

    def get_vlon(
        self,
        v: Union[Tuple[float, float, float], ArrayLike],
        *,
        lon: Union[float, ArrayLike],
        fwd: bool = False,
    ) -> Union[float, ArrayLike]:
        """
        Reimplements ``libgravix2``'s helper function ``grvx_v_lat()`` for NumPy arrays

        :param v: (Batch of) Tuple of first three parameters of ``grvx_v_lon()``
        :param lon: (Batch of) Fourth parameter of ``grvx_v_lat()``
        :param fwd: Bypass reimplementation and forward call to ``libgravix2`` (does not
                    work for NumPy arrays)
        :return: (Batch of) Scaled longitudinal speed
        """
        if fwd:
            vx, vy, vz = v
            return self._helper.get_vlon(float(vx), float(vy), float(vz), float(lon))

        v = np.array(v)
        if v.shape[0] != 3:
            raise ValueError("Last dimension of v has to be of size 3")

        vx = v[0]
        vy = v[1]
        return vx * np.cos(lon) - vy * np.sin(lon)

    @functools.cached_property
    def v_esc(self) -> float:
        """
        Returns the escape velocity

        Calls ``libgravix2``'s helper function ``grvx_v_esc()`` on first call. The
        result is saved and returned on this and all subsequent calls.

        :return: Escape velocity
        """
        return self._helper.get_vesc()

    def v_scrcl(self, r) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``grvx_v_scrcl()``

        :param r: Radius of small circle
        :return: Small circle velocity
        """
        return self._helper.get_vscrcl(r)

    @functools.lru_cache
    def estimate_orb_period(self, *, v0: float, h: float) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``grvx_orb_period()``

        :param v0: Initial velocity
        :param h: Step size
        :return: Orbital period
        """
        return self._helper.get_orb_period(float(v0), float(h))
