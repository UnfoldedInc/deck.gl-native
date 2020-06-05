# Style Guide

### **deck.gl-native** follows [Google C++ Style Guide]([https://google.github.io/styleguide/cppguide.html](https://google.github.io/styleguide/cppguide.html)) with a few notable updates:

- [Trailing Return Syntax](https://google.github.io/styleguide/cppguide.html#trailing_return) - we make use of trailing return syntax across the board
- [Exceptions](https://google.github.io/styleguide/cppguide.html#Exceptions) - our code is exception-tolerant, so it is encouraged to use exceptions wherever it is appropriate. Top-level API provides exception-safe implementations for core functionality
- [File Names](https://google.github.io/styleguide/cppguide.html#File_Names) - file names should use hyphens (`-`) and not underscores (`_`) as delimiters
- [Variable Names](https://google.github.io/styleguide/cppguide.html#Variable_Names) - we use camelCase and not snake_case for variable names. Private variables use a leading underscore instead of a trailing underscore
- [Constant_Names](https://google.github.io/styleguide/cppguide.html#Constant_Names) - constants are declared as UPPERCASE_UNDERSCORED
- [Function Names](https://google.github.io/styleguide/cppguide.html#Function_Names) - function names use camelCase, and do not start with a capitalized case
- [Enumerator_Names](https://google.github.io/styleguide/cppguide.html#Enumerator_Names) - enumerator values are to use the same format as constant names
- [Line Length](https://google.github.io/styleguide/cppguide.html#Line_Length) - allowed line length is 120 characters, up from 80
- [Pointer_and_Reference_Expressions](https://google.github.io/styleguide/cppguide.html#Pointer_and_Reference_Expressions) - */& should be next to type name i.e `int* variable`


### Assertions and Exceptions

- `throw std::logic_error(string)` should be used for when a coding error is detected
- `throw std::runtime_error(string)` should be used for when there is a validation error
