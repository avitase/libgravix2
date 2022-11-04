import numpy as np

from src.gravix2.planet import Planets
from src.gravix2.extensions import observation


def test_game(random, libgravix2):
    planets = Planets(planets=[(-np.pi / 2.0, 0.0), (np.pi / 2.0, 0.0)], lib=libgravix2)
    assert len(planets) == 2

    game = planets.new_game(dt=0.1)
    game.request_launch(
        planet_idx=0, t_start=2.1, dt_ping=1.4, dt_end=2.0, v=1.0, psi=0.0
    )
    game.request_launch(
        planet_idx=0, t_start=1.5, dt_ping=1.6, dt_end=98.5, v=1.0, psi=0.0
    )

    obs = game.tick()
    assert game.t == 1
    assert not obs

    obs = game.tick()
    assert game.t == 2
    assert not obs

    obs = game.tick()
    assert game.t == 3
    assert not obs

    obs = game.tick()
    assert game.t == 4
    assert len(obs) == 2
    assert isinstance(obs[0], observation.Ping)
    assert isinstance(obs[1], observation.Ping)
    assert np.isclose(obs[0].t, 3.1)
    assert np.isclose(obs[1].t, 3.5)

    obs = None
    while not obs and game.t <= 100:
        obs = game.tick()

    assert game.t < 100
    assert len(obs) == 1
    assert isinstance(obs[0], observation.Detonation)
    assert obs[0].t < game.t
    assert obs[0].planet_idx == 1
