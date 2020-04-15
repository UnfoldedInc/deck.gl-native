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

Development can be done on macOS which is the primary environment, or Linux, which is supported mainly for CI testing. (Windows is not a supported dev env.)

**NOTE:** Building using `gcc` currently doesn't work, this is to be updated

## Quickstart

### Xcode (macOS only)

If you haven't already configured Xcode: 
- Install Xcode
- launch it
- accept the license agreement
- run `sudo xcode-select -s /Applications/Xcode.app` (Subsitute with path to your Xcode app if different).

### clang

For macOS:

```sh
brew install clang cmake clang-format lcov
```

### gcc

For macOS:

```sh
brew install gcc cmake clang-format lcov
```

- `scripts/bootstrap.sh` to fetch the dependencies and perform setup. `bootstrap.sh` can/should be called after pulling a branch to ensure environment is up to date
- `scripts/build-clang` or `scripts/build-gcc` to build

## Dependencies

All the dependencies for macOS and Linux are bundled in a dependency repository that's being used as a submodule. Running `scripts/bootstrap.sh`, among other things, fetches all the submodules. Alternatively, you can fetch them by running `git submodule update --init --recursive`.

| Dependency                                                	| Description                                                                         	|
|-----------------------------------------------------------	|-------------------------------------------------------------------------------------	|
| [Google Test](https://github.com/google/googletest)       	| Testing framework                                                                   	|
| [jsoncpp](https://github.com/open-source-parsers/jsoncpp) 	| JSON parser                                                                         	|
| [Range v3](https://github.com/ericniebler/range-v3)       	| C++20 ranges for C++17 compilers                                                    	|
| [fmt](https://fmt.dev/latest/index.html)                  	| C++20 string formatting for C++17 compilers                                         	|
| [arrow](https://github.com/apache/arrow)                  	| Columnar in-memory storage                                                          	|
| [dawn](https://dawn.googlesource.com/dawn)                	| C++ WebGPU implementation with a maturing list of backends for most platforms       	|
| [shaderc](https://github.com/google/shaderc)              	| GLSL shader compilation                                                             	|
| [glfw](https://github.com/glfw/glfw)                      	| Portable library for creating OS windows to render graphics in, and handling events 	|

## Building

```
mkdir build
cd build
cmake ..
make -j 16
```

To use different compilers, set the build options `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` on the `cmake` command line. There is a number of build scripts for different compilers available in `scripts` directory.

## Testing

For Google Test formatted output, run `./tests`.
For CTest formatted output, run `ctest`.
