from dataclasses import dataclass


@dataclass
class Detonation:
    """
    :param planet_idx: The planet index
    :param t: Tick
    """

    planet_idx: int
    t: float


@dataclass
class Ping:
    """
    :param t: Tick
    :param lat: Latitude
    :param lat: Longitude
    """

    t: float
    lat: float
    lon: float
