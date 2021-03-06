import ctypes
from ctypes import c_double, c_int, c_uint, c_void_p, POINTER
from dataclasses import dataclass
from typing import Sequence, Union

import numpy as np
from numpy.typing import ArrayLike

from .helper import Helper
from .planet import Planets


@dataclass(eq=False, order=False, frozen=True)
class Trajectory:
    """
    A sequence of positional and velocity data in Cartesian coordinates

    :param x: Position
    :param v: Velocity
    """

    x: ArrayLike
    v: ArrayLike


class Missile:
    """
    Proxy class for operations on ``libgravix2``'s ``Missile``

    Note that missiles have to be initialized via :func:`gravix2.missile.Missile.set` or
    :func:`gravix2.missile.Missile.launch` before they can be propagated.

    Use :class:`gravix2.missile.Missiles` to create a new instance.

    :param missile: Missile
    :param lib: ``libgravix2`` library
    """

    def __init__(self, *, missile: c_void_p, lib: ctypes.CDLL) -> None:
        self.is_initialized = False
        self._missile = missile
        self._trajectory = None

        init = lib.grvx_init_missile
        init.argtypes = [c_void_p, c_double, c_double, c_double, c_double, c_double]
        init.restype = c_int
        self._init = init

        launch = lib.grvx_launch_missile
        launch.argtypes = [c_void_p, c_void_p, c_uint, c_double, c_double]
        launch.restype = c_int
        self._launch = launch

        propagate = lib.grvx_propagate_missile
        propagate.argtypes = [c_void_p, c_void_p, c_double, POINTER(c_int)]
        propagate.restype = c_uint
        self._propagate = propagate

        self._helper = Helper(lib=lib)

    def set(
        self, *, pos: Sequence[float], orientation: Sequence[float], v: float
    ) -> None:
        """
        Wrapper for ``libgravix2``'s ``grvx_init_missile()`` function

        The initial position and orientation can either be passed as 2D tuples of
        latitudinal and longitudinal components or as 3D tuples in their respective
        Cartesian representation. If passed as 2D tuples ``libgravix2``'s helper
        functions ``grvx_lat()``, ``grvx_lon()``, ``grvx_v_lat()`` and ``grvx_v_lon()``
        are used for conversions.

        :param pos: Initial position
        :param orientation: Initial orientation
        :param v: Initial velocity
        :return: None
        """
        pos = [float(x) for x in pos]
        orientation = [float(dx) for dx in orientation]

        if len(pos) != len(orientation) or len(pos) not in [2, 3]:
            raise ValueError(
                "Position and velocity have either to be given as a pair of latitude "
                "and longitude or in the Cartesian representation"
            )

        if len(pos) == 3:
            x, y, z = pos
            dx, dy, dz = orientation
            lat = self._helper.get_lat(z)
            lon = self._helper.get_lon(x, y)
            dlat = self._helper.get_vlat(dx, dy, dz, lat, lon)
            dlon = self._helper.get_vlon(dx, dy, dz, lon)
        else:
            lat, lon = pos
            dlat, dlon = orientation

        rc = self._init(self._missile, lat, lon, v, dlat, dlon)
        if rc != 0:
            raise RuntimeError("Initializing missile failed")

        self.is_initialized = True

    def launch(
        self, *, planets: Planets, planet_idx: int, v: float, psi: float
    ) -> None:
        """
        Wrapper for ``libgravix2``'s ``grvx_launch_missile()`` function

        :param planets: The planets
        :param planet_idx: The planet index (not ID)
        :param v: Initial velocity
        :param psi: Initial azimuthal position
        :return: None
        """
        rc = self._launch(
            self._missile, planets.handle, planet_idx, float(v), float(psi)
        )
        if rc != 0:
            raise RuntimeError("Launching missile failed")

        self.is_initialized = True

    @property
    def trajectory(self) -> Trajectory:
        """
        Trajectory as generated by :func:`gravix2.missile.Missile.propagate`

        :return: The trajectory
        """
        return self._trajectory

    def propagate(self, planets: Planets, *, h: float) -> bool:
        """
        Wrapper for ``libgravix2``'s ``grvx_propagate_missile()`` function

        On success the :func:`gravix2.missile.Missile.propagate` is filled.

        :param planets: The planets
        :param h: The step size
        :return: Flag indicating whether propagation was stopped prematurely
        """
        if not self.is_initialized:
            raise RuntimeError("Missile is not initialized")

        premature = c_int()
        n = self._propagate(
            self._missile, planets.handle, float(h), ctypes.byref(premature)
        )
        premature = premature.value == 1

        class _Trajectory(ctypes.Structure):
            _fields_ = [
                ("x", c_double * (n * 3)),
                ("v", c_double * (n * 3)),
            ]

        raw = ctypes.cast(self._missile, POINTER(_Trajectory))
        self._trajectory = Trajectory(
            np.ctypeslib.as_array(raw.contents.x).reshape(n, 3),
            np.ctypeslib.as_array(raw.contents.v).reshape(n, 3),
        )

        return premature


class Missiles:
    """
    A sequence of :class:`gravix2.missile.Missile` corresponding to ``libgravix``'s
    ``Missiles``.

    This proxy covers all necessary (de)allocations of ``libgravix``'s
    ``GrvxTrajectoryBatch`` from the user and should behave like a tuple of
    :class:`gravix2.missile.Missile`:

    **Example**

    .. highlight:: python
    .. code-block:: python

        planets = lib.new_planets([...])
        ...
        missiles = lib.new_missiles(3)
        [m.set(pos=(0., 0.), vel=(0., 0.)) for m in missiles]
        m[0].propagate(planets, h=1e-3)
        ...

    Use :func:`gravix2.Gravix2.new_missiles` to create a new sequence of instances.

    :param n: Number of missiles
    :param lib: ``libgravix2`` library
    """

    def __init__(self, n: int, *, lib: ctypes.CDLL) -> None:
        n = int(n)

        new_missiles = lib.grvx_new_missiles
        new_missiles.argtypes = [c_uint]
        new_missiles.restype = c_void_p
        self._handle = new_missiles(n)

        getter = lib.grvx_get_trajectory
        getter.argtypes = [c_void_p, c_uint]
        getter.restype = c_void_p

        self._missiles = [
            Missile(missile=getter(self._handle, i), lib=lib) for i in range(n)
        ]

        delete_missiles = lib.grvx_delete_missiles
        delete_missiles.argtypes = [c_void_p]
        delete_missiles.restype = None
        self._delete_missiles = delete_missiles

    def __len__(self) -> int:
        return len(self._missiles)

    def __getitem__(self, i: Union[int, slice]) -> Missile:
        """
        Returns a missile by index

        :param i: Index of missile or slice
        :return: The missile
        """
        return self._missiles[i]

    def __del__(self) -> None:
        self._delete_missiles(self._handle)
