cmake_minimum_required(VERSION 3.10)
project(MemoryMappedFile)

set(CMAKE_CXX_STANDARD 17)

option(MMAPFILE_BUILD_TESTS "Build tests" ON)

add_subdirectory(source)
if (MMAPFILE_BUILD_TESTS)
    add_subdirectory(test)
endif()
