file(GLOB_RECURSE Sources Source/*.cpp Include/*.h)
set(EngineSources ${Sources})
add_library(Engine ${EngineSources})

target_include_directories(Engine PUBLIC Include)
target_precompile_headers(Engine PUBLIC Include/Core/EnginePCH.hpp)

add_subdirectory(External)

target_compile_definitions(Engine PUBLIC 
        EngineAssets="${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
        EngineShaders="${CMAKE_CURRENT_SOURCE_DIR}/Shaders/")