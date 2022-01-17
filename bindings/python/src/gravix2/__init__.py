"""

We alias import :class:`gravix2.gravix2.Gravix2` as ``load_library`` for convenience,
such that loading and interacting with ``libgravix2.so`` becomes straightforward:

**Example:**

.. highlight:: python
.. code-block:: python

    import gravix2

    lib = gravix2.load_library('path_to_libgravix2.so')
    lat = lib.get_lat(z=45)
    ...
"""
from .gravix2 import Gravix2 as load_library
