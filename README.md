# deck-native-renderer

This is a C++ implementation of deck.gl 


## Dev env

### gcc

```sh
brew install gcc
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

Notes: See `vcpkg` install above.