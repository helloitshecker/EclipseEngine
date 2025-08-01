cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include_directories(${CMAKE_SOURCE_DIR})
find_package(SDL3 CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(Engine_Core
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

add_library(Engine_Renderer
        Engine/Renderer/Renderer.h
        Engine/Renderer/Renderer.c
)

add_library(Engine_System
        Engine/System/Window.c
        Engine/System/Window.h
)

add_library(Engine_Shaders
        Engine/Shaders/Shader.c
        Engine/Shaders/Shader.h

)

add_executable(Engine_Runtime
        Engine/Runtime/Runtime.c
)

target_link_libraries(Engine_System SDL3::SDL3)
target_link_libraries(Engine_Core SDL3::SDL3)

target_link_libraries(Engine_Runtime
        PUBLIC Engine_Core
        PUBLIC Engine_Renderer
        PUBLIC Engine_System
        PUBLIC SDL3::SDL3
        PUBLIC Engine_Shaders
)

add_subdirectory(Engine/Renderer/GL)

add_dependencies(Engine_Runtime Engine_Renderer_EXT_GL)