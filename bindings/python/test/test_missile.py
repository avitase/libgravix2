from src.gravix2.missile import Missiles
from src.gravix2.planet import Planets


def test_missiles(libgravix2):
    planets = Planets([(0.0, 0.0)], lib=libgravix2)

    missiles = Missiles(n=3, lib=libgravix2)
    assert len(missiles) == 3

    mm = [m for m in missiles]
    assert len(mm) == 3

    for m in mm:
        m.launch(planets=planets, planet_idx=0, v=0.0, psi=0.0)

        assert m.trajectory is None

        premature = m.propagate(planets=planets, h=1e-4)
        assert premature
        assert m.trajectory.x.shape == m.trajectory.v.shape
