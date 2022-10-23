#include "chip8.hpp"
#include <iostream>


// it was bc of the texparamtersi!!!!!!!!!!!!!!!!!
// the array that is being used draw into the texture should not be declared/initiazlied right before!!!!!
// gltex2d!!!!!!
// we needed texparatm,ers i!!!!!!!!!!!
// also to declare the array before
// GLPIXELSTORE????? maybe
// TEXPARAMETERSI!!!!!!!!!!!!!!!!!!!!!!
// gltex2d is for intial
// gltexsub is for update
// https://docs.gl/gl3/glPixelStore

// fix imGUI!!!!!!!!!!!!!
// textures work but kinda slow

// save this current appraoach to github!!!
// learn pbos!!

int main(int argc, char* argv[])
{
    if(argc == 1) {
        std::cout << "ERROR: no file/rom specified" << std::endl;
        return 1;
    }

    Chip8 console;

    console.fetch(argv[1]);
    while(console.on()) {
        console.preCycle();
        console.checkTimes();
        console.checkDraw();
        console.postCycle();
    }
    console.end();
    return 0;
}
