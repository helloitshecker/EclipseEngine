cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(Engine
        Engine/Engine.cpp
        Engine/Common/Logger.hpp
        Engine/Common/Memory.hpp
        Engine/Common/Memory.cpp
        Engine/Common/Types.hpp
)
