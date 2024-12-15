find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED HINTS $ENV{VULKAN_SDK}/cmake)
target_link_libraries(Engine PUBLIC SDL2::SDL2)

add_subdirectory(VKB)
target_link_libraries(Engine PUBLIC vk-bootstrap::vk-bootstrap)

add_subdirectory(SimdJSON)
target_link_libraries(Engine PUBLIC simdjson::simdjson)

add_subdirectory(ENTT)
target_link_libraries(Engine PUBLIC EnTT::EnTT)

target_include_directories(Engine PUBLIC STB)

set(FASTGLTF_COMPILE_AS_CPP20 ON CACHE BOOL "Have the library compile as C++20")
add_subdirectory(FastGLTF)
target_link_libraries(Engine PUBLIC fastgltf::fastgltf)

set(BUILD_DOC OFF CACHE BOOL "Build documentation")
set(SKIP_PERFORMANCE_COMPARISON ON CACHE BOOL "Skip building performance sandbox comparison (requires boost)")
set(BUILD_SANDBOX OFF CACHE BOOL "Build sandbox for cereal")
add_subdirectory(Cereal)
target_link_libraries(Engine PUBLIC cereal::cereal)

add_library(imgui STATIC)
target_sources(imgui PRIVATE
        ImGUI/imgui.h
        ImGUI/imgui.cpp
        ImGUI/imgui_demo.cpp
        ImGUI/imgui_draw.cpp
        ImGUI/imgui_widgets.cpp
        ImGUI/imgui_tables.cpp
        ImGUI/backends/imgui_impl_vulkan.cpp
        ImGUI/backends/imgui_impl_sdl2.cpp
)
target_include_directories(imgui PUBLIC ImGUI)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan SDL2::SDL2)
target_link_libraries(Engine PUBLIC imgui)

set( KTX_FEATURE_TESTS OFF CACHE BOOL "Create unit tests.")
add_subdirectory(KTX)
set_target_properties(ktx PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_link_libraries(Engine PUBLIC ktx)



