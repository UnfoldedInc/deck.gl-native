# deck.gl-native

This is an open-source C++ implementation of deck.gl.

> Caveat: this is currently an in-progress effort that is targeting a minimal, proof-of-concept prototype. Even at completion, the initial `deck.gl-native` release is unlikely to meet the requirements of most applications. At this stage, asking for delivery dates and additional features without contributing to implementation or providing additional funding is unlikely to be helpful.

## Scope

- Render to texture (not screen)
- Support deck.gl JSON API
- Layers: Only `ScatterplotLayer`, `LineLayer`, `SolidPolygonLayer`
- Tables: Only Apache Arrow tables will be supported, apps will need to use Arrow libraries if data is not already in Arrow format.
- loader support will be limited to CSV and line-delimited JSON.
- iOS only: will only be tested on iOS (no serious attempts will be done to make deck.gl-native work on other platforms).


## Not Planned

Many features normally considered fundamental by deck.gl applications are not even being addressed at this stage

- No Render to screen
- No Transitions
- No Base map support (e.g. Mapbox integration)
- No Interactivity (event handling, picking etc)
- No Extensive layer catalog
- No Performance Optimizations
- etc


## Software Architecture

A primary concern for the deck.gl-native port, if it matures, is how to keep it develop in sync with the deck.gl JavaScript codebase.

### deck.gl JSON parity (with JavaScript)

The deck.gl JSON API (See the [deck.gl playground](https://deck.gl/playground/)) is the primary "common API" that will be supported by both the JavaScript and C++ implementations of deck.gl. 

This ensures that both C++ and JavaScript evolve in sync on a high level, always providing compatible features (where their layer selections etc overlap). 

### C++ vs JavaScript: API Design Principles

Since the JSON compatiblity requirement helps ensure that the two deck.gl implementations do not diverge, this give some latitude for the actual C++ and JavaScript APIs to be different, to ensure they are both natural to use for programmers with backgrounds in the respective programming language.

This applies more to the API style (how arguments and options are passed etc). Generally, most classes are expected to have one-to-one counterparts in both implementations.

### C++ Module Structure

To make it easier for the deck.gl maintainers to find corresponding code in the deck.gl and the deck.gl-native repo, a similar module structure has been set up.

The JavaScript module structure is documented on the [vis.gl website](https://vis.gl/)

| Module            |.Description |
| --- | --- |
| `probe.gl/core.h` | Platform/Compiler detection, assertions, logging and timers |
| `math.gl/core.h`  | Vectors and Matrices |
| `math.gl/web-mercator.h` | Geospatial math for Web Mercator projection |
| `loaders.gl/csv.h`. | A CSV table loader |
| `loaders.gl/json.h`. | A JSON table loader |
| `luma.gl/core.h` | `Model`, `AnimationLoop` etc implemented on dawn API |
| `luma.gl/webgpu.h` | Internal utilities for working with the WebGPU API (adaptions from the dawn repo) |
| `deck.gl/core.h` | The core deck.gl classes:\ `Deck`, `Layer`, `View`, etc. |
| `deck.gl/layers.h` | The initial layer catalog |
| `deck.gl/json.h` | Classes for parsing (deck.gl) JSON into C++ objects |

At the moment, this module structure only affects how the source code is organized. Only a single `libdeckgl.a` library is provided, that contains the object files from all these modules..

### Apache Arrow

All in-memory table processing is based on the Apache Arrow C++ API. This will be almost invisible to applications if they use the provided loaders, however for more advanced table processing applications may need to work directly with the Arrow API.

To get started with Arrow, useful resources might be:
- [Arrow C++ API Docs](https://arrow.apache.org/docs/cpp/api.html)
- [Arrow Test Suite](https://github.com/apache/arrow/blob/master/cpp/src/arrow/table_test.cc) - Examples of working code.

CSV and JSON table loaders are provided as part of the deck.gl library. To support additional table formats, the envisioned approach is to implement additional "loaders" that load various formats and "convert" the loaded tables to Arrow representation.

### WebGPU via dawn as the Graphics Backend

deck.gl-native is being built on top of the C++ WebGPU API using the [dawn](https://dawn.googlesource.com/dawn) framework.

The dawn framework is a compelling choice for deck.gl:

- The JavaScript version of deck.gl will inevitably move from WebGL to WebGPU, so having both C++ and JavaScript work against a common 3D API will signficantly increase the ease of aligning the JavaScript and C++ code bases.
- The Dawn project has the ambition to provide backends on basically all platforms/rendering APIs of interest, including Vulkan, Metal, D3D12, and OpenGL. Ideally meaning that deck.gl-native itself will only have to implement a single backend, namely dawn.

Note that Dawn is still a work in progress (with different levels of support for different platforms) and there is some risk with this technology choice. However, given the momentum behind WebGPU in browsers, we feel that the prospects are currently looking good.


## Supporting this Effort

This porting project is led by [Unfolded, Inc](www.unfolded.ai), and currently relies on initial funding provided by a customer as well as external contributions. At this stage, this is not an fully or independenly resourced project. It only targets a proof-of-concept prototype, it does not have a maintenance plan and is not set up to address feature requests etc. 

Our hope is to see this project quickly grow into a living part of the core deck.gl project. If this project reaches a sufficient level of completeness / critical mass, the ambition is to make this project part of the main deck.gl project and transfer it to an open governance setup. 


## Setting up a Development Environment

Development can be done on MacOS which is the primary environment, or Linux, which is supported mainly for CI testing. (Windows is not a supported dev env.)

For a quick setup:
- Run `./scripts/bootstrap-repos.sh` **once** (before cloning the deck.gl-native repo) in the folder where you want repos installed. This will clone key repos, set up vcpkg package manager and build the dawn libraries.
- On OSX, run `./scripts/bootstrap.sh` (inside the deck.gl-native repo!) when you need to install any new dependencies (e.g. after pulling master from the deck.gl-native repo).
- On Linux, run `./scripts/bootstrap-linux.sh` instead of `./scripts/bootstrap.sh`.

Detailed instructions below.

### XCode (MacOS only)

If you haven't already configured XCode: 
- Install Xcode
- launch it
- accept the license agreement
- run `sudo xcode-select -s /Applications/Xcode.app` (Subsitute with path to your Xcode app if different).

### gcc

For Mac OSX:

```sh
brew install gcc cmake clang-format lcov
```

### vcpkg (Microsoft C++ Package Manager)

```sh
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh -disableMetrics # Compile
./vcpkg integrate install # Make packages globally availble
```

Notes: 
- Check that the bootstrap script compiles successfully, should generate `vcpkg` binary in root folder
- requires GCC 6 or higher, Apple Clang won't work.

## Depot Tools

Only needed to build dawn: Dawn uses the Chromium build system and dependency management so you need to install [depot_tools](http://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up
)) and add it to the PATH.

```sh
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=$PATH:$PWD/depot_tools # /path/to/depot_tools
```

## Building dawn

See [building dawn[(https://dawn.googlesource.com/dawn/+/HEAD/docs/buiding.md)

```sh
cd dawn
# Bootstrap the gclient configuration
cp scripts/standalone.gclient .gclient
# Fetch external dependencies and toolchains with gclient
gclient sync
# Then generate build files (type `is_debug=true` in text editor)
gn args out/Debug
# Build dawn
ninja -C out/Debug
# Test dawn
./out/Debug/dawn_end2end_tests to run the tests.
# Build examples
cmake -DDAWN_ENABLE_METAL=1 -DDAWN_BUILD_EXAMPLES=1 -DSHADERC_ENABLE_INSTALL=1 .
# Run Examples
cd examples
./Animometer
./CppHelloTriangle
./CubeReflection
./ComputeBoids
```

## Installing Dependencies

First, try running `./scripts/bootstrap-osx.sh` (or `./scripts/bootstrap-linux.sh`).

To install dependencies individually, see below

| Dependency | Install | Documentation
| --- | --- | ---
| [Google Test](https://github.com/google/googletest) | `../vcpkg install gtest` |  Testing framework
| [jsoncpp](https://github.com/open-source-parsers/jsoncpp) | `../vcpkg install jsoncpp` | JSON parser
| [Range v3](https://github.com/ericniebler/range-v3) | `../vcpkg install range-v3` | C++20 ranges for C++17 compilers
| [fmt](https://fmt.dev/latest/index.html) | `../vcpkg install fmt` | C++20 string formatting for C++17 compilers

### Arrow

> There are currently issues with installing Arrow from vcpkg, use the following instructions

- [Arrow](https://arrow.apache.org/install/)

You need to install `libarrow-dev` on Linux,
and `apache-arrow` on Mac OSX:

```sh
brew install apache-arrow
```

### Dawn

[dawn](https://dawn.googlesource.com/dawn) C++ WebGPU implementation with a maturing list of backends for most platforms.

### shaderc

[shaderc](https://github.com/google/shaderc) for GLSL compilation.

### GLFW

[glfw](https://github.com/glfw/glfw) portable library for creating OS windows to render graphics in, and handling events.

On linux a number of dependencies need to be installed for glfw to work:

```sh
sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev
```

## Building

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=<PATH_TO_VCPKG>/scripts/buildsystems/vcpkg.cmake ..
make -j 16
```

To use different compilers, set the build options `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` on the `cmake` command line.

## Testing

For Google Test formatted output, run `./tests`.
For CTest formatted output, run `ctest`.
