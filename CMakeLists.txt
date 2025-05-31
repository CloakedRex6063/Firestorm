cmake_minimum_required(VERSION 3.20)
project(Firestorm)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

option(WITH_SANDBOX "Copy the test sandbox project" ON)

add_subdirectory(Engine)
add_subdirectory(Editor)