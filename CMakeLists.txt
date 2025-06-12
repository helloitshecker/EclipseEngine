cmake_minimum_required(VERSION 3.10)

project(EclipseEngine)

add_subdirectory(thirdParty/glfw)

include_directories(src)
include_directories(src/ExternalIncludes)
include_directories(thirdParty/glfw/include)
include_directories(thirdParty/imgui)
include_directories(thirdParty/implot)

file(GLOB_RECURSE MAIN_SRC src/Eclipse*/*)
file(GLOB SHADERS_SRC src/Shaders/*)

add_library(imgui thirdParty/imgui/imgui.cpp 
thirdParty/imgui/imgui_draw.cpp 
thirdParty/imgui/imgui_widgets.cpp 
thirdParty/imgui/imgui_tables.cpp
thirdParty/imgui/backends/imgui_impl_glfw.cpp
thirdParty/imgui/backends/imgui_impl_opengl3.cpp)

add_library(implot thirdParty/implot/implot.cpp thirdParty/implot/implot_items.cpp)

add_executable(EclipseEngine ${MAIN_SRC} ${SHADERS_SRC})

target_link_libraries(EclipseEngine PUBLIC glfw PUBLIC imgui PUBLIC implot)