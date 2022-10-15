import ctypes
from ctypes import c_double, c_int, c_uint, c_void_p, POINTER
from typing import List, Sequence, Tuple


class Planets:
    """
    Proxy class for operations on ``libgravix2``'s ``Planets``

    New planets are allocated and initialized at the provided coordinates. Planets can
    be accessed and removed by referring to them by their respective indices. Removing
    planets can rearrange the internal representation and indices associations might
    change. Use :func:`gravix2.planet.Planets.planet_id` to get a static and unique
    identification (ID) for each planet. Initially, indices and IDs are identically.

    Use :func:`gravix2.gravix2.Gravix2.new_planets` to create a new instance.

    :param planets: List of latitude and longitude pairs
    :param lib: ``libgravix2`` library
    """

    def __init__(
        self, planets: Sequence[Tuple[float, float]], *, lib: ctypes.CDLL
    ) -> None:
        new_planets = lib.grvx_new_planets
        new_planets.argtypes = [c_uint]
        new_planets.restype = c_void_p
        self.handle = new_planets(len(planets))

        set_planet = lib.grvx_set_planet
        set_planet.argtypes = [c_void_p, c_uint, c_double, c_double]
        set_planet.restype = c_int

        for i, (lat, lon) in enumerate(planets):
            rc = set_planet(self.handle, i, float(lat), float(lon))
            assert rc == 0

        self._planet_pos = list(planets)
        self._planet_id = list(range(len(planets)))

        pop_planet = lib.grvx_pop_planet
        pop_planet.argtypes = [c_void_p]
        pop_planet.restype = c_uint
        self._pop_planet = pop_planet

        delete_planets = lib.grvx_delete_planets
        delete_planets.argtypes = [c_void_p]
        delete_planets.restype = None
        self._delete_planets = delete_planets

        perturb_measurement = lib.grvx_perturb_measurement
        perturb_measurement.argtypes = [
            c_void_p,
            c_int,
            c_double,
            POINTER(c_double),
            POINTER(c_double),
        ]
        perturb_measurement.restype = None
        self._perturb_measurement = perturb_measurement

    @property
    def planet_id(self) -> List[int]:
        """
        Mapping between planet indices and their unique IDs

        :return: List of IDs sorted by index
        """
        return self._planet_id

    @property
    def planet_pos(self) -> List[Tuple[float, float]]:
        """
        List of planet positions

        :return: List of planet positions ordered by index
        """
        return self._planet_pos

    def remove_planet(self, idx: int) -> None:
        """
        Removes a planet by index

        Removal of planets changes the mapping of indices and planet IDs. These changes
        are reflected in :func:`gravix2.planet.Planets.planet_id`.

        :param idx: Planet index
        :return: None
        """
        if idx < 0 or idx >= len(self._planet_id):
            raise IndexError(f"Invalid index {idx}")

        n = self._pop_planet(self.handle)
        assert n == len(self._planet_id) - 1

        self._planet_id[idx] = self._planet_id[-1]
        self._planet_pos[idx] = self._planet_pos[-1]
        self._planet_id.pop()
        self._planet_pos.pop()

    def perturb_measurement(
        self,
        idx: int,
        *,
        lat: float,
        lon: float,
        angular_error: float,
    ) -> Tuple[float, float]:
        """
        Shifts measurement by an angular offset.

        Shifts a measurement taken on the given planet by an angular offset. See
        ``libgravix``'s ``grvx_perturb_measurement()`` for details.

        :param idx: Planet index
        :param lat: Latitudinal position of measurement
        :param lon: Longitudinal position of measurement
        :param angular_error: Angular offset.
        :return: Perturbed measurement
        """
        if idx < 0 or idx >= len(self._planet_id):
            raise IndexError(f"Invalid index {idx}")

        lat, lon = c_double(lat), c_double(lon)
        self._perturb_measurement(
            self.handle, idx, angular_error, ctypes.byref(lat), ctypes.byref(lon)
        )
        return lat.value, lon.value

    def __del__(self):
        self._delete_planets(self.handle)
