import numpy as np

from src.gravix2.planet import Planets


def test_planets(libgravix2):
    planets = Planets(planets=[(1.0, 2.0), (3.0, 4.0), (5.0, 6.0)], lib=libgravix2)
    assert np.all(np.array(planets.planet_id) == np.array([0, 1, 2]))

    planets.remove_planet(1)
    assert np.all(np.array(planets.planet_id) == np.array([0, 2]))

    planets.remove_planet(1)
    assert np.all(np.array(planets.planet_id) == np.array([0]))

    planets.remove_planet(0)
    assert len(planets.planet_id) == 0
