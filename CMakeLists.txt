cmake_minimum_required(VERSION 3.10)
project(EclipseEngine)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# SDL3
if (WIN32)
    # Use vcpkg on Windows
    find_package(SDL3 CONFIG REQUIRED)
else()
    # Use system-installed SDL3 on Linux
    find_package(SDL3 REQUIRED CONFIG)
endif()

# LuaJIT
if (WIN32)
    set(LUAJIT_MAIN_DIR $ENV{VCPKG_ROOT}/packages/luajit_x64-windows)
    set(LUAJIT_INCLUDE_DIR "${LUAJIT_MAIN_DIR}/include/luajit")
    file(GLOB LUAJIT_LINK "${LUAJIT_MAIN_DIR}/lib/lua*.lib")
    list(GET LUAJIT_LINK 0 LUAJIT_LIB)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LUAJIT REQUIRED luajit)
    set(LUAJIT_INCLUDE_DIR ${LUAJIT_INCLUDE_DIRS})
    set(LUAJIT_LIB ${LUAJIT_LIBRARIES})
endif()

message(STATUS "LuaJIT include: ${LUAJIT_INCLUDE_DIR}")
message(STATUS "LuaJIT lib: ${LUAJIT_LIB}")

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
target_link_libraries(Engine_Scripting PRIVATE ${LUAJIT_LIB} Engine_Core Engine_System m)
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
