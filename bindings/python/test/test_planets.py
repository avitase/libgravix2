import numpy as np
import pytest

from src.gravix2.planet import Planets


def test_planets(libgravix2):
    planets = Planets(planets=2, seed=42, min_dist=0.1, lib=libgravix2)
    assert len(planets) == 2
    (lat1, lon1), (lat2, lon2) = planets.planet_pos
    assert -np.pi <= lat1 <= np.pi
    assert -np.pi <= lat2 <= np.pi
    assert -np.pi <= lon1 <= np.pi
    assert -np.pi <= lon2 <= np.pi
    assert not np.isclose(lat1, lat2)
    assert not np.isclose(lon1, lon2)

    planets = Planets(planets=2, seed=None, min_dist=0.1, lib=libgravix2)
    assert len(planets) == 2
    (lat1, lon1), (lat2, lon2) = planets.planet_pos
    assert -np.pi <= lat1 <= np.pi
    assert -np.pi <= lat2 <= np.pi
    assert -np.pi <= lon1 <= np.pi
    assert -np.pi <= lon2 <= np.pi
    assert not np.isclose(lat1, lat2)
    assert not np.isclose(lon1, lon2)

    with pytest.raises(ValueError):
        Planets(planets=2, seed=None, min_dist=None, lib=libgravix2)

    planets = Planets(planets=[(1.0, 2.0), (3.0, 4.0), (5.0, 6.0)], lib=libgravix2)
    assert len(planets) == 3
    assert np.all(np.array(planets.planet_id) == np.array([0, 1, 2]))

    lat, lon = planets.perturb_measurement(0, lat=0.1, lon=0.2, angular_error=0.0)
    assert np.isclose(lat, 0.1)
    assert np.isclose(lon, 0.2)

    planets.remove_planet(1)
    assert np.all(np.array(planets.planet_id) == np.array([0, 2]))

    planets.remove_planet(1)
    assert np.all(np.array(planets.planet_id) == np.array([0]))

    planets.remove_planet(0)
    assert len(planets.planet_id) == 0
