cmake_minimum_required(VERSION 3.29)
project(Firestorm LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Ob0 /MP")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /Ob3 /MP")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -fno-inline")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -finline-functions")
endif ()

if (ENABLE_TIDY)
    add_custom_target(
            clang-format
            COMMAND ${CLANG_FORMAT_BIN}
            -i
            ${SOURCE_FILES}
            COMMENT "Running clang-format on source files"
    )
    add_dependencies(Engine clang-format)
endif ()

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
        "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/*.comp")

find_program(GLSL_VALIDATOR glslc REQUIRED)
foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "Gathering shaders")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Shaders/${FILE_NAME}.spv")
    message(STATUS ${GLSL})
    message(STATUS COMMAND " " ${GLSL_VALIDATOR} " " ${GLSL} " " -o " " ${SPIRV})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(CompileShaders DEPENDS ${SPIRV_BINARY_FILES})