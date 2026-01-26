#include <iostream>
#include <SDL2/SDL.h>
#include "Chip8.h"
#include <thread>

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v
};

int main(int argc, char **argv){

    if (argc != 2){
        std::cout << "Please enter a ROM path." << std::endl;
    }

    Chip8 chip8 = Chip8();

    int height = 512;
    int width = 1024;

    if(width%32 != 0 || height%32 != 0){
        std::cout << "Warning: window is not a multiple of 32 pixels. "
        "Content may not appear as expected." << std::endl;
    }

    SDL_Window* window = NULL;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "Error initializing SDL systems. Please try again." << std::endl;
    }

    window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
         width, height, SDL_WINDOW_SHOWN);

    //Creating object to provide drawing context to window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    //Providing window size to renderer for accurate scaling
    SDL_RenderSetLogicalSize(renderer, width, height);

    //Pixel format ARGB8888 most common for chip8 emulator, often a native window format as well
    //Texture access streaming used for values that change often
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING, 64, 32);

    //Pixel buffer to support conversion between ARGB8888 (32bit) values and the Chip8 8bit values
    uint32_t pixels[64*32];

    //Jump label for reset key press
    startROM:
    //Not necessary for first run, but helps to prevent
    //Visual artifacts on reloads
    memset(chip8.pixels, 0, sizeof(chip8.pixels));
    memset(pixels, 0, sizeof(pixels));
    if(!chip8.LoadROM(argv[1]))
        return 1;

    while(true){
        chip8.executeCycle();

        SDL_Event evt;

        while(SDL_PollEvent(&evt)){  //Go through SDL event queue
            if(evt.type == SDL_QUIT)
                exit(0);

            if(evt.type == SDL_KEYDOWN){
                if(evt.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);
                
                if(evt.key.keysym.sym == SDLK_F1)
                    goto startROM;

                for(int i=0; i<16; i++){
                    if(evt.key.keysym.sym == keymap[i])
                        chip8.key[i] = 1;
                }
            }

            if(evt.type == SDL_KEYUP){
                //No keyup events necessary for exit or reset ROM
                //Only to reset/decrement the keypad values
                for(int i=0; i<16; i++){
                    if(evt.key.keysym.sym == keymap[i])
                        chip8.key[i] = 0;
                }
            }
        }

        if (chip8.drawFlag){
            for(int i = 0; i< 64; i++){
                for(int j = 0; j< 32; j++){
                    pixels[j*64 + i] = chip8.pixels[i][j]? 0xFFFFFFFF : 0xFF000000; 
                }
            }

            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));

            //Clean out and reset the renderer
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            chip8.drawFlag = false;
        }

        // Sleep to slow down emulation speed   
        std::this_thread::sleep_for(std::chrono::microseconds(1800));
    }
}
