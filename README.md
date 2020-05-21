# deck.gl-native

This is an open-source C++ implementation of deck.gl.

![demo](https://i.imgur.com/qHcfFCn.gif)

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

## Supported Platforms

### iOS

- iOS12+ is required when building for a device
- iOS13+ is required when building for a simulator, as `Metal` is available on the simulator starting with iOS13
- Currently has to be built with bitcode disabled (under investigation)

### macOS

- macOS10.14+

#### Linux

- Tested and runs on Ubuntu Bionic

## Software Architecture

To get oriented on the deck.gl software architecture:
- [deck.gl Technical Deep-Dive](https://docs.google.com/presentation/d/1qtXUQzMuIa8NYIKUa1RKfSwvgpeccY-wrPrYqsb_8rE) is a good initial read. Written for the JavaScript version, however most concepts apply directly to the C++ port as well.
- [deck.gl cross-platform architecture](https://docs.google.com/presentation/d/1fzSHJxstSfNQe8VCtLLZvQ8D3OuJAfS7_LqPg0QPPvc) describes approaches to keeping multi-language implementations of deck.gl compatible (still a work-in-progress but gives a sense of the higher level direction).

### API Design Principles and Cross-Platform Concerns

A primary concern for the deck.gl-native port, especially once it matures, is how to keep it develop in sync with the various implementations and variants of the deck.gl API, such as the core JavaScript codebase, the pydeck Python API etc.

Currently the two main guidelines are:

- **deck.gl JSON parity** The deck.gl JSON API (See e.g. [deck.gl playground](https://deck.gl/playground/)) is the "common API" that will be supported by both the JavaScript and C++ implementations of deck.gl. This help ensure that both the C++ and JavaScript renderers evolve in sync, always providing compatible features (when their layer catalogs overlap etc). 
- **module and class parity** Within reason, the same modules, classes (and often even filenames) are used in both implementations. This 1-to-1 mapping really helps developers move quickly between the two code bases. 

Beyond the above rules, it is certainly acceptable C++ and JavaScript APIs to diverge, as long as it makes sense, to ensure they are both natural to use for programmers with backgrounds in the respective programming language. (E.g. functional style UI programming is common in the JS community while C++ programmers on the balance are likely favor a more imperative API style).

### C++ Module Structure

The deck.gl-native code bases exposes a set of C++ "module header files" following a `<framework>/<module>.h` structure. 

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

Remarks:
- The `framework` part of the C++ module header file name corresponds on the JavaScript side to a framework from the [vis.gl](https://vis.gl/) framework suite, and the `module` part corresponds to a submodule in that framework monorepo.
- At the moment, this module structure only affects how the source code is organized. Only a single `libdeckgl.a` library is provided, that contains the object files from all these modules. (In the future this may change and a library for each framework might be provided.)

### Apache Arrow

All in-memory table processing is based on the Apache Arrow C++ API. This will be almost invisible to applications if they use the provided loaders, however for more advanced table processing applications may need to work directly with the Arrow API.

To get started with Arrow, useful resources might be:
- [Arrow C++ API Docs](https://arrow.apache.org/docs/cpp/api.html)
- [Arrow Test Suite](https://github.com/apache/arrow/blob/master/cpp/src/arrow/table_test.cc) - Examples of working code.

CSV and JSON table loaders are provided as part of the deck.gl library. To support additional table formats, the envisioned approach is to implement additional "loaders" that load various formats and "convert" the loaded tables to Arrow representation.

### Graphics Backend: dawn/WebGPU

deck.gl-native is being built on top of the C++ WebGPU API using the [dawn](https://dawn.googlesource.com/dawn) framework.

The dawn framework is a compelling choice for deck.gl:

- The JavaScript version of deck.gl will inevitably move from WebGL to WebGPU, so having both C++ and JavaScript work against a common 3D API will signficantly increase the ease of aligning the JavaScript and C++ code bases.
- The Dawn project has the ambition to provide backends on basically all platforms/rendering APIs of interest, including Vulkan, Metal, D3D12, and OpenGL. Ideally meaning that deck.gl-native itself will only have to implement a single backend, namely dawn.

Note that Dawn is still a work in progress (with different levels of support for different platforms - the prototype is only being tested on iOS) and there is some risk with this technology choice. However, given the momentum behind WebGPU in browsers, we feel that the prospects are currently looking good.


## Supporting this Effort

This porting project is led by [Unfolded, Inc](https://www.unfolded.ai), and currently relies on initial funding provided by a customer as well as external contributions. At this stage, this is not an fully or independenly resourced project. It only targets a proof-of-concept prototype, it does not have a maintenance plan and is not set up to address feature requests etc. 

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

For Google Test formatted output, run `./deckgl-bundle-tests`.
For CTest formatted output, run `ctest`.
