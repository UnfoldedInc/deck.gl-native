# Visual Studio Code development instructions

Our preferred IDE is VS Code.

## To setup debugging

These instructions were written for Linux, where the key binding is CTRL+SHIFT+P.
On Mac OSX it's probably something like COMMAND+SHIFT+P.

* Install the "C/C++" and "CMake Tools" extensions, both from Microsoft.
* Open the deck.gl-native folder and accept the suggestions.
* CTRL+SHIFT+P "CMake: Scan for kits"
* CTRL+SHIFT+P "CMake: Edit User-Local CMake Kits"
* Add a kit based on the highest version of Clang already present:

Relace the C and CXX with the appropriate paths. Replace the toolchainFile
with the path to the toolchain file from your vcpkg installation.
```
  {
    "name": "Clang 6.0.0 with VCPKG",
    "compilers": {
      "C": "/usr/bin/clang-6.0",
      "CXX": "/usr/bin/clang++-6.0"
    },
    "toolchainFile": "<PATH_TO_VCPKG>/scripts/buildsystems/vcpkg.cmake"
  },
```

* CTRL+SHIFT+P "CMake: Select a Kit"
* Select the name of the kit you added.
* CTRL+SHIFT+P "CMake: Set Debug Target"
* CTRL+SHIFT+P "CMake: Debug". Should build and launch the tests under debugger.
