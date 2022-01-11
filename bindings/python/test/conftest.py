import ctypes
from pathlib import Path

import pytest


@pytest.fixture
def libgravix2():
    f = Path(__file__).parent.parent / "libs" / "libgravix2.so"
    assert f.is_file(), f

    return ctypes.cdll.LoadLibrary(str(f.resolve()))
