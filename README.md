# libgravix2 🌎🚀

![Unit tests](https://github.com/avitase/libgravix2/actions/workflows/run_tests.yml/badge.svg)
![Build docs](https://github.com/avitase/libgravix2/actions/workflows/build_doc.yml/badge.svg)

A fast yet precise simulation of conservative, attractive forces acting on point-like particles embedded onto the surface of a unit sphere.
The force fields are generate by static points that are embedded on the surface of the same unit sphere.
We implement different symmetric and symplectic integrators and expose a simple C-API to generate and propagate particles.

This library is meant to be a physics engine for small games, where, e.g., _missiles_ fly in the gravitational force fields of _planets_ in a curved universe.
It is fast, lightweight, and lends itself perfectly to be spawned in many concurrent instances.

## 🚧 WORK IN PROGRESS 🚧
This is all work in progress and any help is highly appreciated.
If you find any bugs or want to improve the documentation, please [submit a pull request](https://github.com/avitase/libgravix2/pulls) or [open an issue](https://github.com/avitase/libgravix2/issues).
There are already open issues and you can help to close them!

## Documentation
Find the documentation of our C-API here: [avitase.github.io/libgravix2/](https://avitase.github.io/libgravix2/) 

Furthermore, our C-API can easily be wrapped in high-level languages.
For example, we provide a minimalistic Python binding here: [avitase.github.io/libgravix2/py-bindings/](https://avitase.github.io/libgravix2/py-bindings/)

## How to build
This is C library with (almost) no external dependencies, except for:
 - C mathematical operations from `math.h`, e.g., `libm.so.6`
 - Standard C library, e.g., `libc.so.6`
 - that's all

Building is straightforward with a decent [`cmake`](https://cmake.org) version:

```
libgravix2$ mkdir -p build/
libgravix2$ cd build/
build$ cmake .. && make
build$ ls src/libgravix2.so
src/libgravix2.so
```

The result is a shared library called `libgravix2.so` in `build/src/`.

There are several configuration options that can be set during compilation either by passing them directly via `-D` to `cmake` or via a graphical tool, such as [`ccmake`](https://cmake.org/cmake/help/latest/manual/ccmake.1.html):
 - `CMAKE_BUILD_TYPE`: `Release` (default) or `Debug`. If enabling unit tests, this has to be set to `Debug`.
 - `ENABLE_TESTING`: `On` or `Off` (default). Generate unit tests and require a `Debug` build. Tests can be run with `ctest` after building.
 - `ENABLE_DOXYGEN`: `On` of `Off` (default). Generate documentation and require a [Doxygen installation](https://www.doxygen.nl/index.html).
 - `POT_TYPE`: `2D` (default) or `3D`.
 - `N_POT`: Approximation order of the force field. Only available if `POT_TYPE` is set to `3D`. (Default: `0`)
 - `TRAJECTORY_SIZE`: Size of trajectory. (Default: `100`)
 - `INT_STEPS`: Number of integration steps between trajectory points. (Default: `10`)
 - `MIN_DIST`: Smallest allowed distance between missiles and planets. (Default: `1` degree.)
 - `P_MIN`: Momentum threshold. (Default: `0.0001`)
 - `COMPOSITION_SCHEME`: `p2s1` , `p4s3` , `p4s5` , `p6s9` or `p8s15` (default).

We refer to our [documentation](https://avitase.github.io/libgravix2/) for more information about these options.
