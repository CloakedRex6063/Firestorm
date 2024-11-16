find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED HINTS $ENV{VULKAN_SDK}/cmake)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()
target_link_libraries(Engine PUBLIC Vulkan::Vulkan)
target_link_libraries(Engine PUBLIC SDL2::SDL2)

add_subdirectory(VKB)
target_link_libraries(Engine PUBLIC vk-bootstrap::vk-bootstrap)

add_subdirectory(SimdJSON)
target_link_libraries(Engine PUBLIC simdjson::simdjson)

set(FASTGLTF_COMPILE_AS_CPP20 ON CACHE BOOL "Have the library compile as C++20")
add_subdirectory(FastGLTF)

target_link_libraries(Engine PUBLIC fastgltf::fastgltf)
target_include_directories(Engine PUBLIC STB)
