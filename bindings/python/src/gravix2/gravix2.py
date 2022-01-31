import ctypes
import functools
from pathlib import Path
from typing import List, Tuple, Union

from . import helper
from . import missile
from . import planet
from . import config


class Gravix2:
    """
    Proxy class for ``libgravix2``

    :param lib: Library path to ``libgravix2.so``
    """

    def __init__(self, lib: Union[str, Path]) -> None:
        lib = Path(lib)
        if not lib.is_file():
            raise ValueError(f"Invalid library path {lib}")

        self._lib = ctypes.cdll.LoadLibrary(str(lib.resolve()))
        self._config = config.get_config(lib=self._lib)
        self._helper = helper.Helper(lib=self._lib)

    @property
    def config(self) -> config.Config:
        """
        Wraps ``libgravix2``'s ``Config`` object

        :return: Static configuration as :class:`gravix2.config.Config` instance
        """
        return self._config

    def new_planets(self, planets: List[Tuple[float, float]]) -> planet.Planets:
        """
        Creates a new set of planets

        :param planets: List of latitude and longitude pairs given in units of degrees
        :return: A new set of planets
        """
        return planet.Planets(planets, lib=self._lib)

    def new_missiles(self, missiles: int) -> missile.Missiles:
        """
        Creates a new set of uninitialized missiles

        :param missiles: Number of missiles
        :return: A new set of missiles
        """
        return missile.Missiles(int(missiles), lib=self._lib)

    def get_lat(self, *, z: float) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``lat()``

        :param z: First parameter of ``lat()``
        :return: Latitude
        """
        return self._helper.get_lat(float(z))

    def get_lon(self, *, x: float, y: float) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``lon()``

        :param x: First parameter of ``lon()``
        :param y: Second parameter of ``lon()``
        :return: Longitude
        """
        return self._helper.get_lon(float(x), float(y))

    def get_vlat(
        self, v: Tuple[float, float, float], *, lat: float, lon: float
    ) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``v_lat()``

        :param v: Tuple of first three parameters of ``v_lat()``
        :param lat: Fourth parameter of ``v_lat()``
        :param lon: Fifth parameter of ``v_lat()``
        :return: Latitudinal speed
        """
        vx, vy, vz = v
        return self._helper.get_vlat(
            float(vx), float(vy), float(vz), float(lat), float(lon)
        )

    def get_vlon(self, v: Tuple[float, float, float], *, lon: float) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``v_lon()``

        :param v: Tuple of first three parameters of ``v_lon()``
        :param lon: Fourth parameter of ``v_lat()``
        :return: (Scaled) longitudinal speed
        """
        vx, vy, vz = v
        return self._helper.get_vlon(float(vx), float(vy), float(vz), float(lon))

    @functools.cached_property
    def v_esc(self) -> float:
        """
        Returns the escape velocity

        Calls ``libgravix2``'s helper function ``v_esc()`` on first call. The result is
        saved and returned on this and all subsequent calls.

        :return: Escape velocity
        """
        return self._helper.get_vesc()

    @functools.lru_cache
    def estimate_orb_period(self, *, v0: float, h: float) -> float:
        """
        Wraps call to ``libgravix2``'s helper function ``orb_period()``

        :param v0: Initial velocity
        :param h: Step size
        :return: Orbital period
        """
        return self._helper.get_orb_period(float(v0), float(h))
