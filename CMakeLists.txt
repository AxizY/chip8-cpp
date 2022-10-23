cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(chip8)

add_executable(chip8 src/main.cpp src/renderer.hpp src/engine.hpp src/shader.hpp)

# glm
find_package(glm REQUIRED)
target_link_libraries(chip8 glm::glm)

# glfw
find_package(glfw3 REQUIRED)
target_link_libraries(chip8 glfw3) 

# glad
add_library(glad include/glad/src/glad.c)
target_include_directories(glad PRIVATE include/glad/include)
target_link_libraries(chip8 glad)

# imgui
add_library(
    imgui STATIC
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_opengl3.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui.cpp
    )
target_link_libraries(chip8 imgui) 

# os-specific stuff
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    target_link_libraries(chip8 ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
endif (APPLE)