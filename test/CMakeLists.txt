cmake_minimum_required(VERSION 3.10)
project(test_mmapfile)
set(CMAKE_CXX_STANDARD 17)

include(cmake_gtest.cmake)

function(SetIncludeDirectories)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../source)
endfunction()

function(SetLinkDirectories)
    link_directories($ENV{GTEST_HOME}/lib)
    link_directories(${CMAKE_BINARY_DIR}/source)
endfunction()

function(SetSourceFiles)
    set(SOURCES
        Test_MemoryMappedFile.cpp
        Test_Main.cpp
        PARENT_SCOPE)
endfunction()

function(LinkLibraries)
    target_link_libraries(${PROJECT_NAME} gtest_main)
    target_link_libraries(${PROJECT_NAME} gmock_main)
    target_link_libraries(${PROJECT_NAME} pthread)
    target_link_libraries(${PROJECT_NAME} mmapfile)
endfunction()

SetIncludeDirectories()
SetSourceFiles()
SetLinkDirectories()
add_executable(${PROJECT_NAME} ${SOURCES})

LinkLibraries()
