#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <iostream>
#include <stdint.h>
#include "renderer.hpp"
#include <iomanip>
#include <cstdlib>

uint8_t fonts[80] {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

class Chip8 {
    private:
        uint8_t memory[4096] {};
        uint8_t v[16] {};
        uint8_t delay = 0; // decrement every frame
        uint8_t timer = 0; // plays beep if not 0
        uint16_t stack[16] {};
        uint16_t I = 0;
        uint8_t SP = 0;
        uint16_t PC = 0x200;
        Display* display;
        int count = 0;

    public:
        bool drawFlag = true;
        Chip8(Display* _display) {
            display = _display;
            srand(time(NULL));
        }

        void fetch(std::string romName) {
            FILE *romFile = fopen(romName.c_str(), "rb");
            fseek(romFile, 0, SEEK_END); // goto end
            int length = ftell(romFile); // get length
            fseek(romFile, 0, SEEK_SET); // goto start
            fread(memory + 0x200, length, 1, romFile); // read the romfile into the memory 0x200
            fclose(romFile);
            for (int i = 0; i < 80; i++) {
                memory[i] = fonts[i]; // load in the font
            }
            for (size_t i = 0; i < 16; i++)
            {
                v[i] = 0;
            }
        }

        void oneCycle() {
            uint16_t opcode = memory[PC] << 8 | memory[PC+1];
            bool incPC = true;
            //std::cout << std::setfill('0') << std::setw(4) << std::hex << opcode << " pc: " << PC << " ";

            // 00E0 clear display
            if(opcode == 0x00E0) {
                display->clear();
                //std::cout << "clear" << std::endl;
            }
            // 1nnn jump
            else if((opcode & 0xF000) == 0x1000) {
                PC = opcode & 0x0FFF;
                incPC = false;
                //std::cout << "jump to " << PC << std::endl;
            }
            // 6xnn set register vx
            else if((opcode & 0xF000) == 0x6000) {
                v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
                //std::cout << "set v" << ((opcode & 0x0F00) >> 8) << " to " << (opcode & 0x00FF) << std::endl;
            }
            // 7xnn add value to vx
            else if((opcode & 0xF000) == 0x7000) {
                v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
                //std::cout << "add " << (opcode & 0x00FF) << " to v" << ((opcode & 0x0F00) >> 8) << std::endl;
            }
            // Annn set index register
            else if((opcode & 0xF000) == 0xA000) {
                I = opcode & 0x0FFF;
                //std::cout << "set index to " << (opcode & 0x0FFF) << std::endl;
            }
            // dxyn display/draw
            else if((opcode & 0xF000) == 0xD000) {
                size_t x = v[(opcode & 0x0F00) >> 8];
                size_t y = v[((opcode & 0x00F0)) >> 4];
                //std::cout << "draw (" << x << ", " << y << ")" << std::endl;
                uint16_t pixel;

                v[0xF] = 0;
                for (size_t i = 0; i < (opcode & 0x000F); i++)
                {
                    pixel = memory[I + i];
                    for (size_t w = 0; w < 8; w++)
                    {
                        if((pixel & (0x80 >> w)) != 0)
                        {
                            if(display->getPixel(x+w, y+i) == 1) {
                                v[0xF] = 1;
                            }
                            bool z = 0;
                            if(display->getPixel(x+w, y+i) == 0) {
                                z = 1;
                            }
                            display->changePixel(x+w, y+i, z);
                        }
                    }
                }
                drawFlag = true;
            }
            // 00EE return
            else if(opcode == 0x00EE) {
                SP--;
                PC = stack[SP];
            }
            // 2nnn call addr
            else if((opcode & 0xF000) == 0x2000) {
                stack[SP] = PC;
                SP++;
                PC = opcode & 0x0FFF;
                incPC = false;
            }
            // 3xkk skip next instruction if vx == kk
            else if((opcode & 0xF000) == 0x3000) {
                if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) { 
                    PC += 2;
                }
            }
            // 4xkk skip if not equal ^^
            else if((opcode & 0xF000) == 0x4000) {
                if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) { 
                    PC += 2;
                }
            }
            // 5xy0 skip if vx == vy
            else if((opcode & 0xF00F) == 0x5000) {
                if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4]) { 
                    PC += 2;
                }
            }
            // 8xy0 set vx = vy
            else if((opcode & 0xF00F) == 0x8000) {
                v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
            }
            // 8xy1 bitwise OR between bits of vx and vy, stored in vx
            else if((opcode & 0xF00F) == 0x8001) {
                v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
            }
            // 8xy2 ^^ but bitwise AND instead
            else if((opcode & 0xF00F) == 0x8002) {
                v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
            }
            // 8xy3 ^^ XOR
            else if((opcode & 0xF00F) == 0x8003) {
                v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
            }
            // 8xy4 ^^ vx += vy, and vF = 1/0 if sum is > 255
            else if((opcode & 0xF00F) == 0x8004) {
                v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
                v[0xF] = v[(opcode & 0x00F0) >> 4] > (0xFF - v[(opcode & 0x0F00) >> 8]);
            }
            // 8xy5 ^^ vx -= vy, if result is negative, vF = 0 else 1
            else if((opcode & 0xF00F) == 0x8005) { ///dfgfg
                v[0xF] = !(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]);
                v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
            }
            // 8xy6 vF is set to the value of the least significiant bit of vx (the right most bit), then vX is integer-divided by 2 (which is right bit-shifting)
            else if((opcode & 0xF00F) == 0x8006) {
                v[0xF] = v[(opcode & 0x0F00) >> 8] & 0b1;
                v[(opcode & 0x0F00) >> 8] >>= 1;
            }
            // 8xyE ^^ but __multiplied by 2__, which results in left __bit-shifting__
            else if((opcode & 0xF00F) == 0x800E) {
                v[0xF] = v[(opcode & 0x0F00) >> 8] >> 7;
                v[(opcode & 0x0F00) >> 8] <<= 1;
            }
            // 8xy7 vx = vy-vx, and store if negative or not in vF
            else if((opcode & 0xF00F) == 0x8007) {
                v[0xF] = !(v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4]);
                v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 4]-v[(opcode & 0x0F00) >> 8];
            }
            // 9xy0 skip if vx not equal vy
            else if((opcode & 0xF00F) == 0x9007) {
                if(v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4]) { 
                    PC += 2;
                }
            }
            // Bnnn jump to nnn + v0
            else if((opcode & 0xF000) == 0xB000) {
                PC = (opcode & 0x0FFF) + v[0];
                incPC = false;
            }
            // Cxkk gen rand num from 0-255, ANDed with kk (since its hex, 2 digits can range from 0-255) and then stored in Vx
            else if((opcode & 0xF000) == 0xC000) {
                v[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            }
            // Ex9E skip if key with value of vX is pressed
            else if((opcode & 0xF0FF) == 0xE09E) {
                if(display->getKey(v[(opcode & 0x0F00) >> 8]) == 1) {
                    PC += 2;
                }
            }
            // ExA1 ^^ skip if key not pressed
            else if((opcode & 0xF0FF) == 0xE0A1) {
                if(display->getKey(v[(opcode & 0x0F00) >> 8]) == 0) {
                    PC += 2;
                }
            }
            // Fx07 set Vx to delay timer
            else if((opcode & 0xF0FF) == 0xF007) {
                v[(opcode & 0x0F00) >> 8] = delay;
            }
            // Fx0A stop all execution, store next key-press in Vx
            else if((opcode & 0xF0FF) == 0xF00A) {
                    bool pressed = false;
                    while (!pressed) {
                        for(int i = 0; i < 16; ++i)
                        {
                            if(display->getKey(i) == 1)
                            {
                                v[(opcode & 0x0F00) >> 8] = i;
                                pressed = true;
                            }
                        }
                    }
            }
            // Fx15 set delay = vX
            else if((opcode & 0xF0FF) == 0xF015) {
                delay = v[(opcode & 0x0F00) >> 8];
            }
            // Fx18 set sound = vX
            else if((opcode & 0xF0FF) == 0xF018) {
                timer = v[(opcode & 0x0F00) >> 8];
            }
            // Fx1E I+=Vx
            else if((opcode & 0xF0FF) == 0xF01E) {
                I += v[(opcode & 0x0F00) >> 8];
            }
            // Fx29 I = loc of the font-sprite in memory thats in vX
            else if((opcode & 0xF0FF) == 0xF029) {
                I = v[(opcode & 0x0F00) >> 8] * 0x5; // since each character is 5 bytes in size
            }
            // Fx33 store bcd (binary representation of decimals), store hundreds digit in I, tens digit at I+1, and ones digit at I+2, in memory
            else if((opcode & 0xF0FF) == 0xF033) {
                memory[I] = v[(opcode & 0x0F00) >> 8]/100; // hundreds place
                memory[I+1] = (v[(opcode & 0x0F00) >> 8]/10) % 10; // tens place
                memory[I+2] = v[(opcode & 0x0F00) >> 8] % 10; // ones place
                // since both v[...] and 100 are integers, integer division will happen,
                // if i wanted decimals, i could just change 100 to 100.0f, to make it a float
            }
            // Fx55 copies v0 to vx into memory, starting at location I
            else if((opcode & 0xF0FF) == 0xF055) {
                for (size_t reg = 0; reg < ((opcode & 0x0F00) >> 8); reg++)
                {
                    memory[I+reg] = v[reg];
                }
            }
            // Fx65 copies bytes starting at location I into register v0 to vx
            else if((opcode & 0xF0FF) == 0xF065) {
                for (size_t reg = 0; reg < ((opcode & 0x0F00) >> 8); reg++)
                {
                    v[reg] = memory[I+reg];
                }
            }

            if (incPC) {
                PC += 2;
            }
        }

        void updateTimers() {
            if(delay > 0) {
                delay--;
            }
            
            if(timer > 0)
            {
                if(timer == 1) {
                    timer--;
                }
            }
        }
};

#endif