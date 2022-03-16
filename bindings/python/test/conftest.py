import ctypes
from pathlib import Path
import numpy as np
import random as rand

import pytest


@pytest.fixture
def random():
    rand.seed(42)
    np.random.seed(42)


@pytest.fixture
def libgravix2():
    f = Path(__file__).parent.parent / "libs" / "libgravix2.so"
    assert f.is_file(), f

    return ctypes.cdll.LoadLibrary(str(f.resolve()))
