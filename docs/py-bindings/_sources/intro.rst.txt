Introduction
============

This is a thin wrapper around the C-API of ``libgravix2``.
Internally, this library uses `Python's ctypes <https://docs.python.org/3/library/ctypes.html>`_ and `NumPy <https://numpy.org>`_ which are the only required dependencies.
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
As most calls are forwarded to their wrapped counterparts in ``libgravix2``, we intentionally kept the documentation for those methods short and refer to the `excellent documentation <https://avitase.github.io/libgravix2/>`_ of ``libgravix2`` instead.

More Examples
-------------

We provide a more elaborated example in `this notebook <https://github.com/avitase/libgravix2/blob/main/bindings/python/example.ipynb>`_.

How to install
==============

We don't (yet) provide a PyPI package but, once you have downloaded the repository, building a wheel package is straightforward.
First, download the repository and navigate to the root directory of the Python binding:

.. code-block:: console

    $ git clone https://github.com/avitase/libgravix2
    $ cd libgravix2/bindings/python/
    python $

Next, install/upgrade the `build package <https://pypi.org/project/build/>`_ from PyPI and build a wheel file

.. code-block:: console

    python $ python3 -m venv venv
    python $ . venv/bin/activate
    (venv) python $ python3 -m pip install --upgrade build
    (venv) python $ python3 -m build
    * Creating venv isolated environment...
    * Installing packages in isolated environment... (setuptools>=42)
    [...]
    Successfully built pygravix2-x.y.tar.gz and pygravix2-x.y-py3-none-any.whl
    (venv) python $

You should now see the ``.whl`` file in ``dist/``. Copy the path as we will need it later

.. code-block:: console

    (venv) python $ ls `pwd`/dist/*.whl
    <PATH-TO-.whl>

That's all. You can now install ``pygravix2`` in any of your projects! For example

.. code-block:: console

    (other_venv) foo $ pip install <PATH-TO-.whl>
    (other_venv) foo $ python3
    Python 3.9.9 (main, Jan  1 1970, 00:00:01)
    [GCC 11.2.0] on linux
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import gravix2
    >>>


How to contribute
=================

This is work in progress and any help is highly welcome and will be appreciated.
Please check your changes by running our test suite with `pytest <https://pytest.org>`_.
Note that one of our fixtures expects to find ``libgravix2.so`` in the ``libs`` directory:

.. code-block:: console

    (venv) python $ mkdir -p libs/ && cd libs/
    (venv) libs $ ln -s ../../../build/src/libgravix2.so libgravix2.so

where ``../../../build/`` is the directory where we have built the C-API as a shared library.
The unit tests can then be triggered from the root directory of the Python binding:

.. code-block:: console

    (venv) python $ pip install --upgrade pytest
    (venv) python $ python -m pytest .
    ============================== test session starts ===============================
    platform linux -- Python 3.9.9, pytest-7.1.0, pluggy-1.0.0
    rootdir: /libgravix2/bindings/python
    plugins: anyio-3.5.0
    collected 7 items

    test/test_helpers.py ....                                                  [ 57%]
    test/test_missile.py .                                                     [ 71%]
    test/test_planets.py .                                                     [ 85%]
    test/test_scrcl.py .                                                       [100%]

    =============================== 7 passed in 0.15s ================================

Contribute your own tests but keep in mind that a unit test should not run for more than one second on a decently equipped machine.
Remember to comment your contribution (w/ or w/o a unit test) and check the formatting of the `generated sphinx documentation <https://www.sphinx-doc.org/>`_ **before** submitting changes:

.. code-block:: console

    (venv) python $ pip install --upgrade sphinx
    (venv) python $ pip install --upgrade sphinx-rtd-theme
    (venv) python $ cd doc && make html
    Running Sphinx v4.4.0
    [...]
    build succeeded.

    The HTML pages are in build/html.

Furthermore, make sure to run the `black formatter <https://github.com/psf/black>`_ over all Python files **before committing any changes!**
