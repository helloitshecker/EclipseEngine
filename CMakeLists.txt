cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Engine/ThirdParty)

add_subdirectory(${THIRDPARTY_DIR}/SDL)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${THIRDPARTY_DIR}/SDL/include)

add_library(Core
        Engine/Core/Window.cpp
        Engine/Core/Window.hpp
        Engine/Core/Types.hpp
        Engine/Core/Error.hpp
        Engine/Core/EventManager.cpp
        Engine/Core/EventManager.hpp)

add_executable(Runtime
        Engine/Runtime/Runtime.cpp)

target_link_libraries(Core PUBLIC SDL3-static)
target_link_libraries(Runtime PUBLIC Core)