#include <stdint.h>
#include <iostream>
#include <fstream>
#include <random>

/*TODO: 
Fleshout opcode descriptions
Wrap values into Chip8 class object
*/

uint8_t fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t memory[0xFFF]; //4096 bytes of memory, or 0xFFF bytes
uint8_t V[16]; //Registers
uint16_t I; //Address Register
uint16_t PC; //Program Counter
uint16_t opcode; //Opcode

uint16_t stack[16]; //Address Stack
uint16_t SP; //Stack Pointer

uint8_t pixels[64][32]; //64x32 Screen with
uint8_t delayTimer;
uint8_t soundTimer;

uint8_t key[16];
bool drawFlag;

/*
Memory Allocation:
0x000-0x1FF - Chip 8 interpreter reserved space
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program RAM
*/


void init(){
    I = 0;
    PC = 0x200; //Starting at 0x200

    //Clear registers and stack
    for(int i = 0; i<sizeof(V); i++){
        V[i] = 0;
        stack[i] = 0;
    }
    opcode = 0;
    SP = 0;

    //Clear screen pixel array
    for(int i = 0; i< sizeof(pixels)*sizeof(pixels[0]); i++){
        pixels[i%sizeof(pixels)][i%(sizeof(pixels[0]))] = 0;
    }

    //Clear memory
    for(int i = 0; i<sizeof(memory); i++){
        memory[i] = 0;
    }

    //Load fontset into memory
    for(int i = 0; i<sizeof(fontset); i++){
        //Some emulators start at 50, but 0 is acceptable
        memory[i] = fontset[i];
    }

    //Seed random value generator using time at init
    srand(time(nullptr));

}

void LoadROM(const char *filePath){
    //Open file and start at end of file
    std::cout << "Loading ROM: " << filePath << std::endl;
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if(file.is_open()){
        //Find file size by checking current position relative to start
        std::streampos size = file.tellg();
        //Allocate space to a buffer that's the size of the file
        char* buffer = new char[size];

        //Set file pointer to beginning with offset of 0 bits, dump file contents into buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        //Check that Chip8 RAM is large enough for ROM
        if(sizeof(memory)-0x200 > size){
            //Load file into memory
            for(int i = 0; i<size; i++){
                memory[i+0x200] = buffer[i];
            }
            std::cout<<"File read successfully" << std::endl;
        }
        else{
            std::cout << "File is too large" << std::endl;
        }

        delete[] buffer;
        return;
    }
}

void FetchHandleOpcode(){
    opcode = memory[PC] << 8 | memory[PC+1]; //Fetching both parts of opcode and combining them with | (or) operation
    
    switch (opcode & 0xF000){

        case 0x0000: //Opcode starts with 0
            switch (opcode & 0x000F){
                case 0x0000: //Opcode 00E0, display clear
                    for(int i = 0; i< sizeof(pixels)*sizeof(pixels[0]); i++){
                        pixels[i/0][i%(sizeof(pixels[0]))] = 0;
                    }
                    PC += 2; //Move to next instruction
                    break;
                case 0x000E: //Opcode 00EE, return
                    PC = stack[SP];
                    SP--;
                    break;
                default:
                    std::cout << "Unknown opcode: " << opcode << std::endl;
            }
            break;
        
        case 0x1000: //Opcode 1NNN, goto NNN
            int address = opcode & 0x0FFF; //Get last three digits of opcode to find jump destination
            PC = address;
            break;
        
        case 0x2000: //Opcode 2NNN, *(0xNNN)() (call subroutine at NNN)
            stack[SP] = PC + 2;//Moving to next instruction to store it
            SP++;
            PC = opcode & 0x0FFF;
            break;
            
        case 0x3000: //Opcode 3XNN, if (Vx == NN)
            int X = (opcode & 0x0F00) >> 8;
            if(V[X] == (opcode & 0x00FF))
                PC += 4;
            else
                PC += 2;
            break;

        case 0x4000: //Opcode 4XNN, if (Vx != NN)
            int X = (opcode & 0x0F00) >> 8;
            if(V[X] != (opcode & 0x00FF))
                PC += 4;
            else
                PC += 2;
            break;

        case 0x5000: //Opcode 5XY0, if (Vx == Vy)
            int X = (opcode & 0x0F00) >> 8;
            int Y = (opcode & 0x00F0) >> 4;
            if(V[X] == V[Y])
                PC += 4;
            else
                PC += 2;
            break;

        case 0x6000: //Opcode 6XNN, Vx = NN
            int X = (opcode & 0x0F00) >> 8;
            V[X] = (opcode & 0x00FF);
            PC += 2;
            break;

        case 0x7000: //Opcode 7XNN, Vx += NN
            int X = (opcode & 0x0F00) >> 8;
            V[X] += (opcode & 0x00FF);
            PC += 2;
            break;

        case 0x8000: //Opcode starts with 8
            switch (opcode & 0x000F){
                case 0x0000: //Opcode 8XY0, Vx = Vy
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    V[X] = V[Y];
                    PC += 2;
                    break;
                case 0x0001: //Opcode 8XY1, Vx |= Vy
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    V[X] |= V[Y];
                    PC += 2;
                    break;
                case 0x0002: //Opcode 8XY2, Vx &= Vy
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    V[X] &= V[Y];
                    PC += 2;
                    break;
                case 0x0003: //Opcode 8XY3, Vx ^= Vy
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    V[X] ^= V[Y];
                    PC += 2;
                    break;
                case 0x0004: //Opcode 8XY4, Vx += Vy
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    int sum = V[X] + V[Y];
                    V[0xF] = (sum > 0xFF) ? 1 : 0;
                    V[X] = sum & 0xFF;
                    PC += 2;
                    break;
                case 0x0005: //Opcode 8XY5, Vx -= Vy
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    V[0xF] = (V[X] > V[Y])? 1 : 0;
                    V[X] -= V[Y];
                    PC += 2;
                    break;
                case 0x0006: //Opcode 8XY6, Vx >>= 1
                    int X = (opcode & 0x0F00) >> 8;
                    V[0xF] = V[X] & 0x1;
                    V[X] = V[X] >> 1;
                    PC += 2;
                    break;
                case 0x0007: //Opcode 8XY7, Vx = Vy - Vx
                    int X = (opcode & 0x0F00) >> 8;
                    int Y = (opcode & 0x00F0) >> 4;
                    V[0xF] = (V[Y] > V[X])? 1 : 0;
                    V[X] = V[Y] - V[X];
                    PC += 2;
                    break;
                case 0x000E: //Opcode 8XYE, Vx <<= 1
                    int X = (opcode & 0x0F00) >> 8;
                    V[0xF] = V[X] >> 7;
                    V[X] = V[X] << 1;
                    PC += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: " << opcode << std::endl;
            }
            break;

        case 0x9000: //Opcode 9XY0, if(Vx != Vy)
            int X = (opcode & 0x0F00) >> 8;
            int Y = (opcode & 0x00F0) >> 4;
            if(V[X] != V[Y])
                PC += 4;
            else
                PC += 2;
            break;

        case 0xA000: //Opcode ANNN, I = NNN
            int address = opcode & 0x0FFF;
            I = address;
            PC += 2;
            break;

        case 0xB000: //Opcode BNNN, PC = V0 + NNN
            int address = opcode & 0x0FFF;
            PC = address + V[0];
            break;

        case 0xC000: //Opcode CXNN, Vx = rand() & NN
            int X = (opcode & 0x0F00) >> 8;
            V[X] = (rand() % (0xFF + 1)) & (opcode & 0x0FF);
            PC += 2;
            break;

        case 0xD000: //Opcode DXYN, draw(Vx, Vy, N)
            int X = (opcode & 0x0F00) >> 8;
            int Y = (opcode & 0x00F0) >> 4;
            int height = (opcode & 0x000F);
            int pixVal;
            V[0xF] = 0;

            //Iterate (height) number of times
            for(int yPos = 0; yPos < height; yPos++){
                //Pull the value of the current row from memory
                pixVal = memory[I + yPos];
                //Iterate the fixed 8 times
                for(int xPos = 0; xPos < 8; xPos++){
                    //Check, for each bit of pixVal, if its respective pixel should be flipped
                    if((pixVal & (0x80 >> xPos)) != 0){
                        //If the pixel is currently on, set V[F] to 1 to indicate a collision
                        if(pixels[X+xPos][Y+yPos] == 1)
                            V[0xF] = 1;
                        //XOR the pixel's value to toggle it on or off
                        pixels[X+xPos][Y+yPos] ^= 1;
                    }
                }
            }

            drawFlag = true;
            PC += 2;
            break;

        case 0xE000: //Opcode starts with E
            switch(opcode & 0x000F){
                case 0x000E: //EX9E, if(key() == Vx)
                    int X = (opcode & 0x0F00) >> 8;
                    if (key[V[X]] != 0)
                        PC += 4;
                    else
                        PC += 2;
                    break;
                case 0x0001: //EXA1, if(key() != Vx)
                    int X = (opcode & 0x0F00) >> 8;
                    if (key[V[X]] == 0)
                        PC += 4;
                    else
                        PC += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: " << opcode << std::endl;
            }
            break;

        case 0xF000: //Opcode starts with F
            switch(opcode & 0x00F0){ //Could check last byte, but checking third creates a more orderly list
                case 0x0000: //FX00
                    switch(opcode & 0x000F){ //Check last byte for unique entries
                        case 0x0007: //Opcode FX07, Vx = get_delay()
                            int X = (opcode & 0x0F00) >> 8;
                            V[X] = delayTimer;
                            break;
                        case 0x000A: //Opcode FX0A, Vx = get_key()
                            int X = (opcode & 0x0F00) >> 8;

                            bool keyPressed = false;
                            //Iterate through key array to see if any were flagged as being pressed
                            for(int i = 0; i< sizeof(key[0]); i++){
                                if(key[i] != 0){
                                    keyPressed = true;
                                    V[X] = i;
                                }
                            }
                            if(!keyPressed)
                                return;
                            PC += 2;
                            break;

                        default:
                            std::cout << "Unknown opcode: " << opcode << std::endl;
                    }    
                case 0x0010: //F010
                    switch(opcode & 0x000F){
                        case 0x0001: //Opcode Fx15, delay_timer(Vx)
                            int X = (opcode & 0x0F00) >> 8;
                            delayTimer = V[X];
                            PC += 2;
                            break;
                        case 0x0008: //Opcode FX18, sound_timer(Vx)
                            int X = (opcode & 0x0F00) >> 8;
                            soundTimer = V[X];
                            PC += 2;
                            break;
                        case 0x000E: //Opcode FX1E, I += Vx
                            int X = (opcode & 0x0F00) >> 8;
                            //Wikipedia states V[F] is unaffected by overflow, so it's left commented out
                            //technically 0xFFFF would overflow but memory is only 0xFFF long
                            //V[0xF] = (I + V[X] > 0xFFF)? 1 : 0; 
                            I += V[X];
                            PC += 2;
                            break;
                        default:
                            std::cout << "Unknown opcode: " << opcode << std::endl;
                    }
                    break;

                case 0x0020: //Opcode FX29, I = sprite_addr[VX]
                    int X = (opcode & 0x0F00) >> 8;
                    I = V[X] * 0x5;
                    PC += 2;
                    break;  

                case 0x0030: //Opcode FX33, set_BCD(Vx)
                    int X = (opcode & 0x0F00) >> 8;
                    memory[I] = V[X] / 100;
                    memory[I+1] =  (V[X] / 10) % 10;
                    memory[I+2] = V[X] % 10;
                    PC += 2;
                    break;
                
                case 0x0050: //Opcode FX55, reg_dump(Vx, &I)
                    int X = (opcode & 0x0F00) >> 8;
                    for(int i = 0; i<=X; i++)
                        memory[I + i] = V[X];
                    //Wikipedia states I will be left unmodified after operation
                    //I += 1;
                    PC += 2;
                    break;

                case 0x0060: //Opcode FX65, reg_load(Vx, &I)
                    int X = (opcode & 0x0F00) >> 8;
                    for(int i = 0; i<=X; i++)
                        V[X] = memory[I + i];
                    //Wikipedia states I will be left unmodified after operation
                    //I += 1;
                    PC += 2;
                    break;
                default:
                    std::cout << "Unknown opcode: " << opcode << std::endl;
            }
            default:
                std::cout << "Unknown opcode: " << opcode << std::endl;
    }

    if(delayTimer > 0){
        delayTimer--;
    }

    if(soundTimer > 0){
        if(soundTimer == 1)
            //play sound
        soundTimer--;
    }
}

