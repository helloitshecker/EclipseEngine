cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src)
include_directories(src/thirdParty/glad)

file(GLOB_RECURSE SRC src/core/* src/platform/* src/graphics/* src/scene/*)

if (MSVC)
add_compile_options(/utf-8)
endif()

add_executable(Engine ${SRC} "src/test.cpp")