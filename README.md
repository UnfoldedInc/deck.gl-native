# deck-native-renderer

This is a C++ implementation of deck.gl 


## Dev env

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

Notes: See `vcpkg` install above.

## Build

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=<PATH_TO_VCPKG>/scripts/buildsystems/vcpkg.cmake ..
make -j
```

To use different compilers, set the build options `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER` on the `cmake` command line.

## Test

For Google Test formatted output, run `./tests`.
For CTest formatted output, run `ctest`.
