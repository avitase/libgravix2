from dataclasses import dataclass

@dataclass
class Detonation:
    planet_id: int
    t: float

@dataclass
class Ping:
    t: float
    lat: float
    lon: float
