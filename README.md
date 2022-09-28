# libgravix2 🌎🚀

![Build docs](https://github.com/avitase/libgravix2/actions/workflows/build_docs.yml/badge.svg)
![Unit tests](https://github.com/avitase/libgravix2/actions/workflows/validate.yml/badge.svg)
![Test coverage](https://codecov.io/gh/avitase/libgravix2/branch/main/graph/badge.svg?token=EGGI4HU1K2)

A fast yet precise simulation of conservative, attractive forces acting on point-like particles embedded onto the surface of a unit sphere.
The force fields are generated by static points that are embedded on the surface of the same unit sphere.
We implement different symmetric and symplectic integrators and expose a simple C-API to generate and propagate particles.

This library is meant to be a physics engine for small games where, e.g., _missiles_ fly in the gravitational force fields of _planets_ in a curved universe.
It is fast, lightweight, and lends itself perfectly to be spawned in many concurrent instances.

## 🚧 WORK IN PROGRESS 🚧
This is all work in progress and any help is highly appreciated.
If you find any bugs or want to improve the documentation, please [submit a pull request](https://github.com/avitase/libgravix2/pulls) or [open an issue](https://github.com/avitase/libgravix2/issues).
There are already open issues, and you can help to close them!

If you plan to contribute, please read the [CONTRIBUTING](CONTRIBUTING.md) guide.

## Documentation
Find the documentation of our C-API here: [avitase.github.io/libgravix2/](https://avitase.github.io/libgravix2/) 

Furthermore, our C-API can easily be wrapped in high-level languages.
For example, we provide a minimalistic Python binding here: [avitase.github.io/libgravix2/py-bindings/](https://avitase.github.io/libgravix2/py-bindings/) and use it to showcase a few simple applications in a [Jupyter notebook](bindings/python/example.ipynb).

## How to build
This is a C library with (almost) no external dependencies, except for:
 - C mathematical operations from `math.h`, e.g., `libm.so.6`
 - Standard C library, e.g., `libc.so.6`
 - that's all

Building is straightforward with [`cmake`](https://cmake.org) and its predefined [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html):

```
libgravix2 $ cmake --preset=release
[...]
-- Configuring done
-- Generating done
-- Build files have been written to: build/release
libgravix2 $ cmake --build build/release
```
You'll now find the shared library under `build/release/libgravix2.so`.
If you want to install the library system-wide, you can either copy this (and the other generated files) from the build directory manually or use CMake to install them automatically.
```
libgravix2 $ cmake --install build/release
```
If you prefer to find the files in non-default locations you have to set `CMAKE_INSTALL_PREFIX` during configuration, e.g.,
```
libgravix2 $ cmake --preset=release -DCMAKE_INSTALL_PREFIX=/some/path
libgravix2 $ cmake --build build/release       (as before)
libgravix2 $ cmake --install build/release     (as before)
```

There are more configuration options that can be set during compilation either by passing them directly via `-D` to `cmake` (as shown before with `CMAKE_INSTALL_PREFIX`) or via a graphical tool, such as [`ccmake`](https://cmake.org/cmake/help/latest/manual/ccmake.1.html):
 - `CMAKE_BUILD_TYPE`: `Release` (default) or `Debug`. If enabling unit tests, this has to be set to `Debug`.
 - `ENABLE_TESTING`: `On` or `Off` (default). Generate unit tests and require a `Debug` build. Tests can be run with `ctest` after building.
 - `ENABLE_DOXYGEN`: `On` of `Off` (default). Generate documentation and require a [Doxygen installation](https://www.doxygen.nl/index.html).
 - `GRVX_POT_TYPE`: `2D` (default) or `3D`.
 - `GRVX_N_POT`: Approximation order of the force field. Only available if `POT_TYPE` is set to `3D`. (Default: `0`)
 - `GRVX_TRAJECTORY_SIZE`: Size of trajectory. (Default: `100`)
 - `GRVX_INT_STEPS`: Number of integration steps between trajectory points. (Default: `10`)
 - `GRVX_MIN_DIST`: Smallest allowed distance between missiles and planets. (Default: `1` degree.)
 - `GRVX_COMPOSITION_SCHEME`: `p2s1` , `p4s3` , `p4s5` , `p6s9` or `p8s15` (default).

Have a look into our [documentation](https://avitase.github.io/libgravix2/) for more information about these options.

## CMake package
We also export a CMake package upon installation to be used with the [`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html) command of CMake:

* Package name: `libgravix2`
* Target name: `libgravix2::libgravix2`

Example usage:

```cmake
find_package(libgravix2 REQUIRED)

# Declare the imported target as a build requirement using PRIVATE, where
# project_target is a target created in the consuming project
target_link_libraries(
    project_target PRIVATE
    libgravix2::libgravix2
)
```
