file(GLOB_RECURSE Sources Source/*.cpp Include/*.h)
set(EngineSources ${Sources})
add_library(Engine ${EngineSources})

target_include_directories(Engine PUBLIC Include)
target_precompile_headers(Engine PUBLIC Include/Tools/EnginePCH.hpp)

add_subdirectory(External)

target_compile_definitions(Engine PUBLIC 
        EngineAssets="${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
        EngineShaders="${CMAKE_CURRENT_SOURCE_DIR}/Shaders/")

if (ENABLE_TIDY)
    set_target_properties(Engine PROPERTIES CXX_CLANG_TIDY "clang-tidy")
    add_custom_target(
            clang-format
            COMMAND ${CLANG_FORMAT_BIN}
            ${SOURCE_FILES}
            COMMENT "Running clang-format on source files"
    )
    add_dependencies(Engine clang-format)
endif ()