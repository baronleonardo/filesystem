cmake_minimum_required(VERSION 3.21)
project(filesystem C)
set(C_STANDARD_REQUIRED 17)

option(static "enable static building" OFF)
option(tests  "enable tests" OFF)

add_subdirectory(lib)
if(tests)
    add_subdirectory(test)
endif()