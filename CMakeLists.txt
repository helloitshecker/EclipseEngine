cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
project(EclipseEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include_directories(${CMAKE_SOURCE_DIR})
include_directories($ENV{VULKAN_SDK}/Include)

find_package(SDL3 CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(mio CONFIG REQUIRED)

add_library(Core
    Engine/Core/Error.hpp
    Engine/Core/EventManager.hpp
    Engine/Core/EventManager.cpp
    Engine/Core/FileSystem.hpp
    Engine/Core/FileSystem.cpp
    Engine/Core/Types.hpp
    Engine/Core/Window.hpp
    Engine/Core/Window.cpp
    Engine/Core/JobSystem.hpp
    Engine/Core/VirtualFS.cpp
    Engine/Core/VirtualFS.hpp
)

add_library(Graphics
    Engine/Graphics/RenderDevice.cpp
    Engine/Graphics/RenderDevice.hpp
    Engine/Graphics/ShaderManager.cpp
    Engine/Graphics/ShaderManager.hpp
)

add_executable(Runtime
    Engine/Runtime/Runtime.cpp
)

target_link_libraries(Core
        PUBLIC SDL3::SDL3
        PUBLIC TBB::tbb
        PUBLIC mio::mio
        PUBLIC mio::mio-headers
)
target_link_libraries(Graphics
        PUBLIC Core
        PUBLIC SDL3::SDL3
        PUBLIC volk::volk
        PUBLIC volk::volk_headers
        PUBLIC TBB::tbb
        PUBLIC unofficial::shaderc::shaderc
)
target_link_libraries(Runtime
        PUBLIC Core
        PUBLIC Graphics
        PUBLIC TBB::tbb
        PUBLIC SDL3::SDL3
)
