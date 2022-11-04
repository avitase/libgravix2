import ctypes
from ctypes import c_double, c_int, c_uint, c_void_p, POINTER
from typing import List, Union

from .observation import Detonation, Ping


class Game:
    """
    TODO
    """

    def __init__(self, *, planets: c_void_p, dt: float, lib: ctypes.CDLL) -> None:
        self.planets = planets
        self.dt = dt
        self.t = 0

        count_planets = lib.grvx_count_planets
        count_planets.argtypes = [c_void_p]
        count_planets.restype = c_uint
        self.n_planets = count_planets(planets)

        init_game = lib.grvx_init_game
        init_game.argtypes = [c_void_p]
        init_game.restype = c_void_p
        self.handle = init_game(planets)

        delete_game = lib.grvx_delete_game
        delete_game.argtypes = [c_void_p]
        delete_game.restype = None
        self._delete_game = delete_game

        request_launch = lib.grvx_request_launch
        request_launch.argtypes = [c_void_p, c_uint, c_void_p, c_double]
        request_launch.restype = c_int
        self._request_launch = request_launch

        observe_or_tick = lib.grvx_observe_or_tick
        observe_or_tick.argtypes = [c_void_p, POINTER(c_uint)]
        observe_or_tick.restype = c_void_p
        self._observe_or_tick = observe_or_tick

    def request_launch(
        self,
        *,
        planet_idx: int,
        t_start: float,
        dt_ping: float,
        dt_end: float,
        v: float,
        psi: float,
    ) -> None:
        """
        TODO

        :param planet_idx:
        :param t_start:
        :param dt_ping:
        :param dt_end:
        :param v:
        :param psi:
        :return:
        """

        class Request(ctypes.Structure):
            _fields_ = [
                ("t_start", c_double),
                ("dt_ping", c_double),
                ("dt_end", c_double),
                ("v", c_double),
                ("psi", c_double),
            ]

        req = Request(t_start, dt_ping, dt_end, v, psi)
        rc = self._request_launch(self.handle, planet_idx, ctypes.byref(req), self.dt)
        if rc != 0:
            raise RuntimeError(f"Unsuccessful request. Return code: {rc}")

    def tick(self) -> List[Union[Detonation, Ping]]:
        """
        TODO

        :return:
        """

        class Observation(ctypes.Structure):
            _fields_ = [
                ("planet_id", c_uint),
                ("t", c_double),
                ("lat", c_double),
                ("lon", c_double),
            ]

        observations = []

        done = False
        while not done:
            t = c_uint()
            obs = self._observe_or_tick(self.handle, ctypes.byref(t))
            self.t = t.value

            done = not obs
            if obs:
                obs = ctypes.cast(obs, POINTER(Observation)).contents
                observations.append(
                    Detonation(planet_id=obs.planet_id, t=obs.t)
                    if obs.planet_id < self.n_planets
                    else Ping(t=obs.t, lat=obs.lat, lon=obs.lon)
                )

        return observations

    def __del__(self):
        self._delete_game(self.handle)
