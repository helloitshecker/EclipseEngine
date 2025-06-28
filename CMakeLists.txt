cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/Engine/ThirdParty)

add_subdirectory(${THIRD_PARTY_DIR}/SDL)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${THIRD_PARTY_DIR}/SDL/include)
include_directories(${THIRD_PARTY_DIR}/glad/include)

file(GLOB_RECURSE ENGINE_FILES ${CMAKE_SOURCE_DIR}/Engine/*)
list(FILTER ENGINE_FILES EXCLUDE REGEX ".*/ThirdParty/.*")

add_executable(EclipseEngine ${ENGINE_FILES} ${THIRD_PARTY_DIR}/glad/src/glad.c)

target_link_libraries(EclipseEngine SDL3)
