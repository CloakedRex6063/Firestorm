file(GLOB_RECURSE Sources Source/*.cpp Include/*.h Include/*.hpp)
set(SandboxSources ${Sources})
add_executable(Sandbox ${SandboxSources})

target_link_libraries(Sandbox PRIVATE Engine)
target_include_directories(Sandbox PUBLIC Include)

target_compile_definitions(Engine PUBLIC
        GameConfig="${CMAKE_CURRENT_SOURCE_DIR}/Config/"
        GameAssets="${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

file(GLOB_RECURSE ADDITIONAL_TEXTURES
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.png"
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.jpg"
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.jpeg")

set(SUBDIR_TEXTURE_FILES ${ADDITIONAL_TEXTURES} PARENT_SCOPE)
