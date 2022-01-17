Introduction
============

This is a thin wrapper around the C-API of ``libgravix2`` exposed by our proxy class
:class:`gravix2.gravix2.Gravix2` as returned by ``gravix2.load_library``:

**Example:**

.. highlight:: python
.. code-block:: python

    import gravix2

    lib = gravix2.load_library('path_to_libgravix2.so')

From here on one can use ``lib``, for example, to create new :class:`gravix2.missile.Missiles`
by using :func:`gravix2.gravix2.Gravix2.new_missiles`

**Example:**

.. highlight:: python
.. code-block:: python

    missiles = lib.new_missiles(n=3)

Typically, users shouldn't instantiate any of our provided proxy classes manually but use the
methods exposed by :class:`gravix2.gravix2.Gravix2`.
As most calls are forwarded to their wrapped counterparts in ``libgravix2`` we intentionally kept
the documentation for those methods short and refer to excellent documentation of ``libgravix2``
instead.