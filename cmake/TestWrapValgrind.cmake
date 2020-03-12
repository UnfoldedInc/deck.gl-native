# Copyright 2018 Unfolded, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Support code for wrapping tests or other executables with Valgrind.

if(__test_wrap_valgrind)
  return()
endif()
set(__test_wrap_valgrind 1)

include(CMakeDependentOption)

find_program(VALGRIND valgrind)
cmake_dependent_option(WRAP_VALGRIND "Wrap tests in valgrind" OFF
                       "VALGRIND" OFF)
if(WRAP_VALGRIND)
    # TEST_WRAPPER could be used to configure the Valgrind parameters, or
    # to use a different wrapper entirely.
    set(TEST_WRAPPER ${VALGRIND} --track-origins=yes --leak-check=full --error-exitcode=99 CACHE STRING
        "Wrapper executable for tests and benchmarks")
    mark_as_advanced(TEST_WRAPPER)
    # Convert from semicolon separated list of values to a form
    # that can be used by a shell.
    string(REPLACE ";" " " TEST_WRAPPER_STR "${TEST_WRAPPER}")
endif()

