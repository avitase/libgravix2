Introduction
============

This is a thin wrapper around the C-API of ``libgravix2``.
Internally, this library uses `Python's ctypes <https://docs.python.org/3/library/ctypes.html>`_ and `numpy <https://numpy.org>`_ which are the only required dependencies.
If provided with the path to ``libgravix2.so``, its API is exposed by our proxy class
:class:`gravix2.gravix2.Gravix2` as returned by ``gravix2.load_library``:

**Example:**

.. highlight:: python
.. code-block:: python

    import gravix2

    lib = gravix2.load_library('path_to_libgravix2.so')

From here on, one can use ``lib``, for example, to create new :class:`gravix2.missile.Missiles` by using :func:`gravix2.gravix2.Gravix2.new_missiles`

**Example:**

.. highlight:: python
.. code-block:: python

    missiles = lib.new_missiles(n=3)

Typically, users shouldn't instantiate any of our provided proxy classes manually but use the methods exposed by :class:`gravix2.gravix2.Gravix2`.
As most calls are forwarded to their wrapped counterparts in ``libgravix2``, we intentionally kept the documentation for those methods short and refer to excellent documentation of ``libgravix2`` instead.

How to contribute
=================

This is work in progress and any help is highly welcome and will be appreciated.
Please run our test suite with `pytest <https://pytest.org>`_, e.g., by calling ``python -m pytest``, and test the formatting of our `sphinx documentation <https://www.sphinx-doc.org/>`_ before submitting changes.
