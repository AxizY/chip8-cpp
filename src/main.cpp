#include <glad/glad.h>
#include "shader.hpp"
#include "renderer.hpp"
#include "engine.hpp"
#include "chip8.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

// its bc it needs poll events, but swapbuffers need to be every draw call

// smth wrong with the font

int main(int argc, char *argv[])
{
    if(argc == 1) {
        std::cout << "ERROR: no file/rom specified" << std::endl;
        return 1;
    }

    Screen<64, 32> screen;
    Display display(&screen);
    Chip8 console(&display);

    console.fetch(argv[1]);
    while(!display.shouldClose()) {
        if(display.checkCycle()) {
            console.oneCycle();
        }
        if(display.checkTimer()) {
            console.updateTimers();
        }
        if(console.drawFlag) {
            display.loop();
            console.drawFlag = false;
        }
        display.pollEvents();
    }
    display.end();
    return 0;
}