FILE(GLOB_RECURSE ENGINE_SOURCES Source/*.cpp)
add_library(Engine STATIC ${ENGINE_SOURCES})
target_include_directories(Engine PUBLIC Include)
target_precompile_headers(Engine PUBLIC Source/Core/EnginePCH.hpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Engine PUBLIC FS_DEBUG)
else ()
    target_compile_definitions(Engine PUBLIC FS_RELEASE)
endif ()

add_subdirectory(Shaders)
add_subdirectory(External)