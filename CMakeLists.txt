cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

include_directories(${CMAKE_SOURCE_DIR})
find_package(SDL3 CONFIG REQUIRED)

add_library(Engine_Core STATIC
        Engine/Core/Types.h
        Engine/Core/Memory.c
        Engine/Core/Memory.h
        Engine/Core/Debug.h
        Engine/Core/Events.h
        Engine/Core/Time.h
        Engine/Core/Time.c
        Engine/Core/FileSystem.c
        Engine/Core/FileSystem.h
)

add_library(Engine_Renderer STATIC
        Engine/Renderer/Renderer.h
        Engine/Renderer/Renderer.c
)

add_library(Engine_System
        Engine/System/Window.c
        Engine/System/Window.h
)

add_library(Engine_Scripting STATIC
        Engine/Scripting/Scripting.h
        Engine/Scripting/Scripting.c

)

add_executable(Engine_Runtime
        Engine/Runtime/Runtime.c
)

target_include_directories(Engine_Scripting PRIVATE $ENV{VCPKG_ROOT}/packages/luajit_x64-windows/include)

target_link_libraries(Engine_System SDL3::SDL3 Engine_Core)
target_link_libraries(Engine_Core SDL3::SDL3)
target_link_libraries(Engine_Scripting PRIVATE $ENV{VCPKG_ROOT}/packages/luajit_x64-windows/lib/lua51.lib PRIVATE Engine_Core PRIVATE Engine_System)

target_link_libraries(Engine_Runtime
        PUBLIC Engine_Core
        PUBLIC Engine_Renderer
        PUBLIC Engine_System
        PUBLIC Engine_Scripting
        PUBLIC SDL3::SDL3
)

add_subdirectory(Engine/Renderer/GL)

add_dependencies(Engine_Runtime Engine_Renderer_EXT_GL)