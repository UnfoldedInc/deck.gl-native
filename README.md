# deck.gl-native

This is an open-source C++ implementation of deck.gl 

> Caveat: this is currently an in-progress effort that is targeting a minimal, proof-of-concept prototype. Even at completion, the initial `deck.gl-native` release is unlikely to meet the requirements of most applications. At this stage, asking for delivery dates and additional features without contributing to implementation or providing additional funding is unlikely to be helpful.

## Scope

- Render to texture (not screen)
- Support deck.gl JSON API
- Layers: Only `ScatterplotLayer`, `LineLayer`, `SolidPolygonLayer`
- Tables: Only Apache Arrow tables will be supported, apps will need to use Arrow libraries if data is not already in Arrow format.
- loader support will be limited to CSV and line-delimited JSON.


## Not Planned

Many features normally considered fundamental by deck.gl applications are not even being addressed at this stage

- No Render to screen
- No Transitions
- No Base map support (e.g. Mapbox integration)
- No Interactivity (event handling, picking etc)
- No Extensive layer catalog
- No Performance Optimizations
- etc

## Supporting this Effort

This porting project is led by [Unfolded, Inc](www.unfolded.ai), and currently relies on initial funding provided by a customer as well as external contributions. At this stage, this is not an fully or independenly resourced project. It only targets a proof-of-concept prototype, it does not have a maintenance plan and is not set up to address feature requests etc. 

Our hope is to see this project quickly grow into a living part of the core deck.gl project. If this project reaches a sufficient level of completeness / critical mass, the ambition is to make this project part of the main deck.gl project and transfer it to an open governance setup. 


## Software Architecture

A primary concern for the deck.gl-native port, if it matures, is how to keep it develop in sync with the deck.gl JavaScript codebase.

### deck.gl JSON parity (with JavaScript)

The deck.gl JSON API (See the [deck.gl playground](https://deck.gl/playground/)) is the primary "common API" that will be supported by both the JavaScript and C++ implementations of deck.gl. 

This ensures that both C++ and JavaScript evolve in sync on a high level, always providing compatible features (where their layer selections etc overlap). 

### C++ vs JS API

Since the JSON compatiblity requirement helps ensure that the two deck.gl implementations do not diverge, this give some latitude for the actual C++ and JavaScript APIs to be different, to ensure they are both natural to use for programmers with backgrounds in the respective programming language.

### C++ Module Structure

To simplify for JavaScript maintainers to work in the deck.gl-native repo, a similar module structure has been set up, and similar file naming conventions are used.

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
| `deck.gl/core.h` | | The core deck.gl classes: `Deck`, `Layer`, `View`, etc. |
| `deck.gl/layers.h` | The initial layer catalog |
| `deck.gl/json.h` | Classes for parsing (deck.gl) JSON into C++ objects |

At the moment, this module structure only affects how the source code is organized. Only a single `libdeckgl.a` library is provided, that contains the object files from all these modules..

### Apache Arrow

All in-memory table processing is based on the Apache Arrow C++ API. This will be almost invisible to applications if they use the provided loaders, however for more advanced table processing applications may need to work directly with the Arrow API.

To get started with Arrow, useful resources might be:
- [Arrow C++ API Docs](https://arrow.apache.org/docs/cpp/api.html)
- [Arrow Test Suite](https://github.com/apache/arrow/blob/master/cpp/src/arrow/table_test.cc) - Examples of working code.

CSV and JSON table loaders are provided as part of the deck.gl library. To support additional table formats, the envisioned approach is to implement additional "loaders" that load various formats and "convert" the loaded tables to Arrow representation.


## Setting up a Development Environment

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

## Dependencies

### Google Test

- [Google Test](https://github.com/google/googletest)

```sh
../vcpkg install gtest
```

### jsoncpp

- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

```sh
../vcpkg install jsoncpp
```

### range v3

Used as a placeholder until clang/gcc compilers become C++20 compatible.

- [Range v3](https://github.com/ericniebler/range-v3)

```sh
../vcpkg install range-v3
```

### Arrow

- [Arrow](https://arrow.apache.org/install/)

You need to install `libarrow-dev` on Linux,
and `apache-arrow` on Mac OSX:

```sh
brew install apache-arrow
```

## Graphics Dependencies

deck.gl-native is built on top of the C++ WebGPU API using the Dawn framework.

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
make -j
```

To use different compilers, set the build options `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` on the `cmake` command line.

## Testing

For Google Test formatted output, run `./tests`.
For CTest formatted output, run `ctest`.
