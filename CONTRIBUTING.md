# Contributing
Here is some wisdom to help you build and test this project as a developer and potential contributor.

## Code of Conduct
 - Please checkout (and respect) our [formatting guide](.clang-format) before submitting any changes.
 - Always code as if the guy who ends up maintaining your code will be a violent psychopath who knows where you live.
 - Code for readability.
 - You will be judged by your contributions first, and your sense of humor second.
 - Nobody owes you anything.

## Developer mode
Build system targets that are only useful for developers of this project are hidden if the `libgravix2_DEVELOPER_MODE` option is disabled.
Enabling this option makes tests and other developer targets and options available.
Not enabling this option means that you are a consumer of this project and thus you have no need for these targets and options.

Developer mode is always set to `ON` in CI workflows.

### Presets
This project makes use of [CMake's presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) to simplify the process of configuring the project.
As a developer, you are recommended to always have the [latest CMake version](https://cmake.org/download/) installed to make use of the latest Quality-of-Life additions.

You have a few options to pass `libgravix2_DEVELOPER_MODE` to the configure command, but this project prefers to use presets.

As a developer, you should create a `CMakeUserPresets.json` file at the root of the project.
We provide an example configuration in [CMakeUserPresets.json.example](CMakeUserPresets.json.example) that you can use as a starting point, e.g.,
```
libgravix2 $ cp CMakeUserPresets.json.example CMakeUserPresets.json
```
This file is also the perfect place in which you can put all sorts of things that you would otherwise want to pass to the configure command in the terminal.
Note, however, your `CMakeUserPresets.json` should **not** be checked into source control!

Furthermore, note that we enable [`clang-tidy`](https://clang.llvm.org/extra/clang-tidy/) and [`cppcheck`](https://cppcheck.sourceforge.io/) by default in our provided [`CMakeUserPresets.json.example`](CMakeUserPresets.json.example).
Make sure to either install these tools or remove them from your personal presets before configuring the project.

### Configure, build and test
We use the [Catch2 `v2.x`](https://github.com/catchorg/Catch2/tree/v2.x) framework for testing and CMake will scan your system for a valid installation.
If you use [CMakeUserPresets.json.example](CMakeUserPresets.json.example) then you can configure, build and test the project respectively with the following commands from the project root:
```
libgravix2 $ cmake --preset=mydev-debug2D
libgravix2 $ cmake --build --preset=mydev-debug2D
libgravix2 $ cd build/mydev2D/ && ctest .
```
or
```
libgravix2 $ cmake --preset=mydev-debug3D
libgravix2 $ cmake --build --preset=mydev-debug3D
libgravix2 $ cd build/mydev3D/ && ctest .
```
If you are using a compatible editor or IDE, you will also be able to select the above created user presets for automatic integration.

Please note that both the build and test commands accept a `-j` flag to specify the number of jobs to use, which should ideally be specified to the number of threads your CPU has.
You may also want to add that to your preset using the `jobs` property, see the [presets documentation](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) for more details.

### Developer mode targets
We provide additional targets you may invoke using the build command from above, with an additional `-t <target>` flag:

 - `coverage`: Available if `ENABLE_COVERAGE` is enabled.
This target processes the output of the previously run tests when built with coverage configuration.
The commands this target runs can be found in the `COVERAGE_TRACE_COMMAND` and `COVERAGE_HTML_COMMAND` cache variables.
The trace command produces an info file by default, which can be submitted to services with CI integration.
The HTML command uses the trace command's output to generate an HTML document to `build/coverage_html` by default.

 - `docs`: Available if `BUILD_DOCS` is enabled.
Builds the documentation using Doxygen.
The output will go to `build/docs`.

### Examples
In developer mode we automatically enable `BUILD_EXAMPLES` which will build the examples under [`examples/`](examples/).
For release builds type
```
libgravix2 $ cmake --preset=release -DBUILD_EXAMPLES=ON
libgravix2 $ cmake --build build/release

libgravix2 $ build/release/examples/scrcl 
0 3.33067e-15 1.11022e-15
```
The example `scrcl` (shown above) can be used as a benchmark:
It simulates the trajectory of a missile on a small circle for a fixed number of iterations and returns three values:
 - `1` if the simulation has ended prematurely (error), else `0`.
 - Error of radius prediction (should be small).
 - Error of velocity prediction (should be small).
