from src import gravix2


def test_config(libgravix2):
    grvx = gravix2.load_library(libgravix2)
    cfg = grvx.config

    assert cfg.pot_type in ["2D", "3D"]
    if cfg.pot_type == "2D":
        assert cfg.n_pot is None
    else:
        assert cfg.n_pot is not None

    assert cfg.trajectory_size > 0

    assert cfg.int_steps > 0

    assert cfg.min_dist >= 0.0

    assert cfg.n_stages > 0

    assert cfg.composition_scheme.endswith(str(cfg.n_stages))
