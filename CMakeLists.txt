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

# os-specific stuff
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    target_link_libraries(chip8 ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
endif (APPLE)