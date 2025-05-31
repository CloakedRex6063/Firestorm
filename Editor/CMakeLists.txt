FILE(GLOB_RECURSE EDITOR_SOURCES Source/*.cpp)
add_executable(Editor ${EDITOR_SOURCES})
target_include_directories(Editor PUBLIC Include)
target_link_libraries(Editor PUBLIC Engine)
