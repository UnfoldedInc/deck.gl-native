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
- etc

## Supporting this Effort

The ongoing port is led by [Unfolded, Inc](www.unfolded.ai), and currently relies heavily on initial funding provided by a customer as well as external contributions. At this stage, this is not an independenly resourced project. It does not have a maintenance plan and is not set up to address external feature requests. 

That said, our hope is to see this project quickly grow into a living part of the core deck.gl project. If this happens, Unfolded Inc, may consider transferring this to a foundation / open governance setup. 

However, at this stage the project has yet to attain a sufficient level of completeness / critical mass. And additional work will happen only through direct engineering contributions or by providing additional funding. 


## Setting up Development Environment

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
