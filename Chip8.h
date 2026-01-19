#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>

class Chip8{
    private:
        uint8_t memory[0xFFF]; //4096 bytes of memory, or 0xFFF bytes
        uint8_t V[16]; //Registers
        uint16_t I; //Address Register
        uint16_t PC; //Program Counter
        uint16_t opcode; //Opcode

        uint16_t stack[16]; //Address Stack
        uint16_t SP; //Stack Pointer

        uint8_t delayTimer;
        uint8_t soundTimer;
        
        void init();

    public:
        uint8_t pixels[64][32]; //64x32 Screen with
        uint8_t key[16];
        bool drawFlag;

        Chip8();
        ~Chip8();

        bool LoadROM(const char *filePath);
        void executeCycle();
};


#endif