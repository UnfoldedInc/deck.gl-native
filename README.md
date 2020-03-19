# deck.gl-native

> Caveat: this is currently an in-progress effort that is targeting a minimal, proof-of-concept prototype. Even at completion, `deck.gl-native` is highly unlikely to meet the requirements of most applications. Asking for additional features without contributing implementation or providing funding is not helpful.

This is an open-source C++ implementation of deck.gl 

## Scope

- Render to texture (not screen)
- Only `ScatterplotLayer`, `LineLayer`, `SolidPolygonLayer`
- Apache Arrow tables only.
- loader support will be limited to CSV and line-delimited JSON. 


## Not Planned

Many features normally considered fundamental by deck.gl applications are not even being addressed at this stage

- No Render to screen
- No Transitions
- No Base map support (e.g. Mapbox integration)
- No Interactivity (event handling, picking etc)
- No Extensive layer catalog
- No etc etc etc

## Supporting this Effort

The ongoing port is led by Unfolded, Inc through funding provided by a customer and external contributions. At this stage, this is not an independenly resourced project set up to address feature requests. 

We would love to see this project grow into a living part of the main deck.gl project, potentially transferring this to a foundation/open governance setup. 

However, at this stage additional work will happen only through direct engineering contributions or by providing additional funding. 


## Setting up Development Environment

### gcc

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
- requires gcc 6 or higher, apple clang won't work.

## Dependencies

### jsoncpp

- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

```sh
../vcpkg install jsoncpp
```

### Google Test

- [Google Test](https://github.com/google/googletest)

```sh
../vcpkg install gtest
```

### Arrow

- [Arrow](https://arrow.apache.org/install/)

You need to install `libarrow-dev` on Linux,
and `apache-arrow` on Mac OSX (Brew).

Notes: See `vcpkg` install above.

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
