cmake_minimum_required(VERSION 3.10)

project(Engine_Renderer_EXT_GL)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

#include_directories(../../..)
#include_directories(./glad/include)

add_library(Engine_Renderer_EXT_GL SHARED Driver/RenderDevice.c Driver/RenderDevice.h glad/glad.c)
target_link_libraries(Engine_Renderer_EXT_GL PUBLIC Engine_Core PUBLIC Engine_System PUBLIC SDL3::SDL3)