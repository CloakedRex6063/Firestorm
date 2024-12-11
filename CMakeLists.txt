cmake_minimum_required(VERSION 3.29)
project(Firestorm LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building on Windows")
    add_compile_definitions(FS_PLATFORM_WINDOWS)
else()
    message(STATUS "Building on an unsupported OS: ${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(FS_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_definitions(FS_RELEASE)
endif ()

add_subdirectory(Engine)
add_subdirectory(Sandbox)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/*.frag"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/*.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/*.comp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/*.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/*.mesh")

find_program(GLSL_VALIDATOR glslc REQUIRED)
message(STATUS "Gathering shaders")
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    get_filename_component(FILE_EXT ${GLSL} EXT)
    set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/${FILE_NAME}.spv")
    if(${FILE_EXT} STREQUAL ".mesh")
        set(SHADER_STAGE "-fshader-stage=mesh")
    else ()
        set(SHADER_STAGE "")
    endif()
    message(STATUS ${GLSL})
    message(STATUS COMMAND " " ${GLSL_VALIDATOR} " " ${SHADER_STAGE} " " ${GLSL} " " --target-spv=spv1.6 " " -o " " ${SPIRV})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} ${SHADER_STAGE} ${GLSL} --target-spv=spv1.6 -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()


add_custom_target(CompileShaders DEPENDS ${SPIRV_BINARY_FILES})

file(GLOB_RECURSE TEXTURE_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Assets/*.png"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Assets/*.jpg"
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Assets/*.jpeg")
list(APPEND TEXTURE_SOURCE_FILES ${SUBDIR_TEXTURE_FILES})

message(STATUS "Gathering textures")
foreach (Texture ${TEXTURE_SOURCE_FILES})
    get_filename_component(TexturePath ${Texture} DIRECTORY)
    get_filename_component(TextureName ${Texture} NAME_WE)
    set(KTX ${TexturePath}/${TextureName}.ktx2)
    message(STATUS ${KTX})
    add_custom_command(
            OUTPUT ${KTX}
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/Compression/img2ktx.exe -o ${KTX} -f BC1 ${Texture}
            DEPENDS ${Texture})
    list(APPEND KTX_FILES ${KTX})
endforeach ()

add_custom_target(ConvertTexture DEPENDS ${KTX_FILES})