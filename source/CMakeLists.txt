cmake_minimum_required(VERSION 3.17)
project(mmapfile)
set(CMAKE_CXX_STANDARD 20)

function(SetIncludeDirectories)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(SetSourceFiles)
    set(SOURCES MemoryMappedFile.cpp PARENT_SCOPE)
endfunction()

SetIncludeDirectories()
SetSourceFiles()

add_library(${PROJECT_NAME} SHARED ${SOURCES})