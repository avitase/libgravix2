import ctypes
from ctypes import c_char_p, c_double, c_int, POINTER
from dataclasses import dataclass
from typing import Optional


@dataclass(eq=False, order=False, frozen=True)
class Config:
    """
    ``libgravix2`` static configuration

    :param pot_type: Same as ``POT_TYPE``
    :param n_pot: Same as ``N_POT``
    :param trajectory_size: Same as ``TRAJECTORY_SIZE``
    :param int_steps: Same as ``INT_STEPS``
    :param min_dist: Same as ``MIN_DIST``
    :param composition_scheme: Same as ``COMPOSITION_SCHEME``
    """
    pot_type: str
    n_pot: Optional[int]
    trajectory_size: int
    int_steps: int
    min_dist: float
    composition_scheme: str


def get_config(*, lib: ctypes.CDLL) -> Config:
    """
    Returns ``libgravix2``'s static ``Config``

    :param lib: ``libgravix2`` library
    :return: The static configuration
    """
    class _Config(ctypes.Structure):
        _fields_ = [
            ("pot_type", c_char_p),
            ("n_pot", c_int),
            ("trajectory_size", c_int),
            ("int_steps", c_int),
            ("min_dist", c_double),
            ("composition_scheme", c_char_p),
        ]

    get_cfg = lib.get_config
    get_cfg.argtypes = None
    get_cfg.restype = POINTER(_Config)

    cfg = get_cfg()
    config = Config(
        cfg.contents.pot_type.decode("ascii"),
        cfg.contents.n_pot if cfg.contents.n_pot >= 0 else None,
        cfg.contents.trajectory_size,
        cfg.contents.int_steps,
        cfg.contents.min_dist,
        cfg.contents.composition_scheme.decode("ascii"),
    )

    free_config = lib.free_config
    free_config.argtypes = [POINTER(_Config)]
    free_config.restype = None

    free_config(cfg)

    return config
