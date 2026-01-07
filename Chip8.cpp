#include <stdint.h>

uint8_t memory[0xFFF]; //4096 bytes of memory, or 0xFFF bytes
uint8_t V[16]; //Registers
uint16_t I; //Address Register
uint16_t PC; //Program Counter

uint16_t stack[16]; //Address Stack
uint16_t SP; //Stack Pointer

uint8_t pixels[64][32]; //64x32 Screen with
uint8_t delayTimer;
uint8_t soundTimer;

/*
Memory Allocation:
0x000-0x1FF - Chip 8 interpreter reserved space
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program RAM
*/


void CPUReset(){
    I = 0;
    PC = 0x200; //Starting at 0x200
    for(int i = 0; i<sizeof(V); i++){
        V[i] = 0;
    }
}

void FetchHandleOpcode(){
    uint16_t opcode = memory[PC] << 8 | memory[PC+1]; //Fetching both parts of opcode and combining them with | (or) operation
    
    switch (opcode & 0xF000){

        case 0x0000: //Opcode starts with 0
            switch (opcode & 0x000F){
                case 0x0000: //Opcode 00E0, display clear
                    break;
                case 0x000E: //Opcode 00EE, return
                    break;
            }
        
        case 0x1000: //Opcode 1NNN, goto NNN
            break;
        
        case 0x2000: //Opcode 2NNN, *(0xNNN)() (call subroutine at NNN)
            break;
            
        case 0x3000: //Opcode 3XNN, if (Vx == NN)
            break;

        case 0x4000: //Opcode 4XNN, if (Vx != NN)
            break;

        case 0x500: //Opcode 5XY0, if (Vx == Vy)
            break;

        case 0x600: //Opcode 6XNN, Vx = NN
            break;

        case 0x700: //Opcode 7XNN, Vx += NN
            break;

        case 0x800: //Opcode starts with 8
            switch (opcode & 0x000F){
                case 0x0000: //Opcode 8XY0, Vx = Vy
                    break;
                case 0x0001: //Opcode 8XY1, Vx |= Vy
                    break;
                case 0x0002: //Opcode 8XY2, Vx &= Vy
                    break;
                case 0x0003: //Opcode 8XY3, Vx ^= Vy
                    break;
                case 0x0004: //Opcode 8XY4, Vx += Vy
                    break;
                case 0x0005: //Opcode 8XY5, Vx -= Vy
                    break;
                case 0x0006: //Opcode 8XY6, Vx >>= 1
                    break;
                case 0x0007: //Opcode 8XY7, Vx = Vy -Vx
                    break;
                case 0x000E: //Opcode 8XYE, Vx <<= 1
                    break;

            }

        case 0x900: //Opcode 9XY0, if(Vx != Vy)
            break;

        case 0xA000: //Opcode ANNN, I = NNN
            break;

        case 0xB000: //Opcode BNNN, PC = V0 + NNN
            break;

        case 0xC000: //Opcode CXNN, Vx = rand() & NN
            break;

        case 0xD000: //Opcode DXYN, draw(Vx, Vy, N)
            break;

        case 0xE000: //Opcode starts with E
            switch(opcode & 0x000F){
                case 0x000E: //EX9E, if(key() == Vx)
                    break;
                case 0x0001: //EXA1, if(key() != Vx)
                    break;
            }

        case 0xF000: //Opcode starts with F
            switch(opcode & 0x00F0){ //Could check last byte, but third has a more orderly list
                case 0x0000: //FX00
                    switch(opcode & 0x000F){ //Check last byte for unique entries
                        case 0x0007: //Opcode FX07, Vx = get_delay()
                            break;
                        case 0x000A: //Opcode FX0A, Vx = get_key()
                            break;
                    }    
                case 0x0010: //F010
                    switch(opcode & 0x000F){
                        case 0x0001: //Opcode Fx15, delay_timer(Vx)
                            break;
                        case 0x0008: //Opcode FX18, sound_timer(Vx)
                            break;
                        case 0x000E: //Opcode FX18, I += Vx
                            break;
                    }

                case 0x0030: //Opcode FX33, set_BCD(Vx)
                    break;
                
                case 0x0050: //Opcode FX55, reg_dump(Vx, &I)
                    break;

                case 0x0060: //Opcode FX65, reg_load(Vx, &I)
                    break;
            }
    }
}

