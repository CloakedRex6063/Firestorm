FetchContent_Declare(
        GLM
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 2d4c4b4
)
FetchContent_MakeAvailable(GLM)
target_link_libraries(Engine PUBLIC glm::glm)

FetchContent_Declare(
        GLAZE
        GIT_REPOSITORY https://github.com/stephenberry/glaze
        GIT_TAG v5.2.0)
FetchContent_MakeAvailable(GLAZE)
target_link_libraries(Engine PUBLIC glaze::glaze)


add_library(STB STB/stb_image.cpp)
target_include_directories(STB PUBLIC STB)
target_link_libraries(Engine PUBLIC STB)

add_library(DXIL INTERFACE)
target_include_directories(DXIL INTERFACE DXIL/inc)
target_link_libraries(Engine PUBLIC DXIL)
target_link_libraries(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/DXIL/lib/dxcompiler.lib)
target_link_libraries(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/DXIL/lib/dxil.lib)

FetchContent_Declare(
        DX12Headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers
        GIT_TAG v1.615.0
)
FetchContent_MakeAvailable(DX12Headers)
target_link_libraries(Engine PUBLIC Microsoft::DirectX-Headers)
target_link_libraries(Engine PUBLIC dxgi.lib d3d12.lib)