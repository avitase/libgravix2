import ctypes
import random
from ctypes import c_double, c_int, c_uint, c_void_p, POINTER
from typing import List, Optional, Sequence, Tuple, Union

from .extensions.game import Game


class Planets:
    """
    Proxy class for operations on ``libgravix2``'s ``Planets``

    New planets are allocated and initialized either at the provided coordinates or
    randomly. Planets can be accessed and removed by referring to them by their
    respective indices. Removing planets can rearrange the internal representation and
    indices associations might change. Use :func:`gravix2.planet.Planets.planet_id` to
    get a static and unique identification (ID) for each planet. Initially, indices and
    IDs are identically.

    Use :func:`gravix2.gravix2.Gravix2.new_planets` to create a new instance.

    :param planets: Number of planets or list of latitude and longitude pairs. If this
                    parameter is an integer, this many planets are initialized randomly.
    :param seed: If planets are initialized randomly a seed can be passed. If ``None``
                 a random seed will be generated.
    :param min_dist: If planets are initialized randomly, a min. distance is required.
    :param lib: ``libgravix2`` library
    """

    def __init__(
        self,
        planets: Union[int, Sequence[Tuple[float, float]]],
        *,
        seed: Optional[int] = None,
        min_dist: Optional[float] = None,
        lib: ctypes.CDLL,
    ) -> None:
        self.lib = lib

        new_planets = lib.grvx_new_planets
        new_planets.argtypes = [c_uint]
        new_planets.restype = c_void_p

        if isinstance(planets, int):
            if min_dist is None:
                self.handle = None  # disable __del__()
                raise ValueError(
                    "Random initialization of planets requires a min. distance"
                )

            if planets < 1:
                self.handle = None  # disable __del__()
                raise ValueError("Number of planets has to be at least 1")

            if seed is None:
                seed = random.getrandbits(32)

            seed = c_uint(seed)

            rnd_init = lib.grvx_rnd_init_planets
            rnd_init.argtypes = [c_void_p, POINTER(c_uint), c_double]
            rnd_init.restype = c_uint

            self.handle = new_planets(planets)
            rnd_init(self.handle, ctypes.byref(seed), c_double(min_dist))

            get_planet = lib.grvx_get_planet
            get_planet.argtypes = [
                c_void_p,
                c_uint,
                POINTER(c_double),
                POINTER(c_double),
            ]
            get_planet.restype = c_int

            self._planet_pos = []
            for i in range(planets):
                lat, lon = c_double(), c_double()
                rc = get_planet(
                    self.handle, c_uint(i), ctypes.byref(lat), ctypes.byref(lon)
                )
                assert rc == 0

                self._planet_pos.append((lat.value, lon.value))

        else:
            if min_dist is not None:
                raise Warning(
                    "Planets are initialized explicitly and parameter "
                    "`min_dist` is ignored"
                )

            if seed is not None:
                raise Warning(
                    "Planets are initialized explicitly and parameter "
                    "`seed` is ignored"
                )

            set_planet = lib.grvx_set_planet
            set_planet.argtypes = [c_void_p, c_uint, c_double, c_double]
            set_planet.restype = c_int

            self.handle = new_planets(len(planets))
            for i, (lat, lon) in enumerate(planets):
                rc = set_planet(self.handle, i, float(lat), float(lon))
                assert rc == 0

            self._planet_pos = list(planets)

        self._planet_id = list(range(len(self._planet_pos)))

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

    def __len__(self) -> int:
        return len(self._planet_pos)

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

    def new_game(self, *, dt: float) -> Game:
        """
        Creates a new game instance

        Creates a new game instance from ``libgravix2``'s game extension.

        :param dt: Step size of simulation
        :return: A new game
        """
        return Game(planets=self.handle, dt=dt, lib=self.lib)

    def __del__(self):
        if self.handle is not None:
            self._delete_planets(self.handle)
