cmake_minimum_required(VERSION 3.10)
project(EclipseEngine)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

# Find SDL3 via vcpkg
find_package(SDL3 CONFIG REQUIRED)

# Manually locate LuaJIT (no config file provided by vcpkg)
set(LUAJIT_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/luajit-2.1")
set(LUAJIT_LIB "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libluajit-5.1.a")

if(NOT EXISTS "${LUAJIT_INCLUDE_DIR}/lua.h")
    message(FATAL_ERROR "Could not find LuaJIT headers in ${LUAJIT_INCLUDE_DIR}")
endif()
if(NOT EXISTS "${LUAJIT_LIB}")
    message(FATAL_ERROR "Could not find LuaJIT library in ${LUAJIT_LIB}")
endif()

# Engine_Core
add_library(Engine_Core STATIC
    ${CMAKE_SOURCE_DIR}/Engine/Core/Types.h
    ${CMAKE_SOURCE_DIR}/Engine/Core/Memory.c
    ${CMAKE_SOURCE_DIR}/Engine/Core/Memory.h
    ${CMAKE_SOURCE_DIR}/Engine/Core/Debug.h
    ${CMAKE_SOURCE_DIR}/Engine/Core/Events.h
    ${CMAKE_SOURCE_DIR}/Engine/Core/Time.h
    ${CMAKE_SOURCE_DIR}/Engine/Core/Time.c
    ${CMAKE_SOURCE_DIR}/Engine/Core/FileSystem.c
    ${CMAKE_SOURCE_DIR}/Engine/Core/FileSystem.h
)
target_include_directories(Engine_Core PUBLIC ${CMAKE_SOURCE_DIR})

# Engine_Renderer
add_library(Engine_Renderer STATIC
    ${CMAKE_SOURCE_DIR}/Engine/Renderer/Renderer.h
    ${CMAKE_SOURCE_DIR}/Engine/Renderer/Renderer.c
)
target_include_directories(Engine_Renderer PUBLIC ${CMAKE_SOURCE_DIR})

# Engine_System
add_library(Engine_System
    ${CMAKE_SOURCE_DIR}/Engine/System/Window.c
    ${CMAKE_SOURCE_DIR}/Engine/System/Window.h
)
target_include_directories(Engine_System PUBLIC ${CMAKE_SOURCE_DIR})

# Engine_Scripting
add_library(Engine_Scripting STATIC
    ${CMAKE_SOURCE_DIR}/Engine/Scripting/Scripting.h
    ${CMAKE_SOURCE_DIR}/Engine/Scripting/Scripting.c
)
target_include_directories(Engine_Scripting
    PUBLIC ${CMAKE_SOURCE_DIR}
    PRIVATE ${LUAJIT_INCLUDE_DIR}
)

# Engine_Runtime executable
add_executable(Engine_Runtime
    ${CMAKE_SOURCE_DIR}/Engine/Runtime/Runtime.c
)
target_include_directories(Engine_Runtime PUBLIC ${CMAKE_SOURCE_DIR})

# Linking
target_link_libraries(Engine_Scripting PRIVATE ${LUAJIT_LIB} Engine_Core Engine_System)
target_link_libraries(Engine_System PRIVATE SDL3::SDL3 Engine_Core)
target_link_libraries(Engine_Core PRIVATE SDL3::SDL3)
target_link_libraries(Engine_Runtime
    PUBLIC Engine_Core
    PUBLIC Engine_Renderer
    PUBLIC Engine_System
    PUBLIC Engine_Scripting
    PUBLIC SDL3::SDL3
)

# Extra renderer backend
add_subdirectory(${CMAKE_SOURCE_DIR}/Engine/Renderer/GL)
add_dependencies(Engine_Runtime Engine_Renderer_EXT_GL)
