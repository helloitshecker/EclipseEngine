cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

add_subdirectory(src/thirdParty/glfw)
add_subdirectory(src/thirdParty/fmt)

include_directories(src)
include_directories(src/thirdParty/glad)

file(GLOB_RECURSE SRC src/core/* src/platform/* src/graphics/* src/scene/*)

if (MSVC)
add_compile_options(/utf-8)
endif()

add_executable(Engine ${SRC} "src/test.cpp")

target_link_libraries(Engine glfw fmt::fmt)