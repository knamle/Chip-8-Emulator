//#include   // OpenGL graphics and input
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <chrono>

#include "chip8.hpp"

chip8 myChip8;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int CHIP8_W = 64;
constexpr int CHIP8_H = 32;

void setupGraphics() {

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " 
                    << SDL_GetError()
                        << std::endl;
    } else {
        //Create window
        window = SDL_CreateWindow( "nomnamnom", 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    SCREEN_WIDTH, 
                                    SCREEN_HEIGHT, 
                                    SDL_WINDOW_SHOWN );
        if( window == NULL ) {
            std::cerr << "Window could not be created! SDL_Error: " 
                        << SDL_GetError()
                         << std::endl;
        } else {
            renderer = SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
            );
            if (!renderer) {
                std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
                SDL_DestroyWindow(window);
                SDL_Quit();
                return;
            }

            SDL_RenderSetLogicalSize(renderer, CHIP8_W, CHIP8_H);
            
        }
    }
}

void drawGraphics() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i=0; i<32; ++i) {
        for (int j=0; j<64; ++j) {
            if (chip8::gfx[j * 32 + i]) {
                SDL_RenderDrawPoint(renderer, j, i);
            }
        }
    }
    SDL_RenderPresent(renderer);
    // after done drawing set needtoredraw = false?
}

void mapSDLKeyToChip8(SDL_Keycode key, uint8_t &chip8key) {
    switch (key) {
        case SDLK_1: chip8key = 0x1; break;
        case SDLK_2: chip8key = 0x2; break;
        case SDLK_3: chip8key = 0x3; break;
        case SDLK_4: chip8key = 0xC; break;

        case SDLK_q: chip8key = 0x4; break;
        case SDLK_w: chip8key = 0x5; break;
        case SDLK_e: chip8key = 0x6; break;
        case SDLK_r: chip8key = 0xD; break;

        case SDLK_a: chip8key = 0x7; break;
        case SDLK_s: chip8key = 0x8; break;
        case SDLK_d: chip8key = 0x9; break;
        case SDLK_f: chip8key = 0xE; break;

        case SDLK_y: chip8key = 0xA; break; 
        case SDLK_x: chip8key = 0x0; break;
        case SDLK_c: chip8key = 0xB; break;
        case SDLK_v: chip8key = 0xF; break;

        default: chip8key = 0xFF; break;
    }
}

int main (int argc, char **argv) {
    setupGraphics();
    //setupInput();

    myChip8.initialize();

    myChip8.loadGame("pong");

    SDL_Event e; 
    bool quit = false; 
    //int i = 0;

    chip8::gfx[50] = 1;
    chip8::gfx[1000] = 1;

    using clock = std::chrono::steady_clock;
    const double OPS_PER_SEC = 60;
    const auto STEP = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<double>(1.0 / OPS_PER_SEC)
    );

    auto last = clock::now();

    while( quit == false ){
        auto now = clock::now();

        while( SDL_PollEvent( &e ) ){ 
            uint8_t chip8key;
            mapSDLKeyToChip8(e.key.keysym.sym, chip8key);

            switch( e.type ) {
                case SDL_KEYDOWN:
                    printf( "Key press detected\n" );
                    if (chip8key != 0xff) myChip8.setKeys(chip8key, false);
                    break;

                case SDL_KEYUP:
                    printf( "Key release detected\n" );

                    if (chip8key != 0xff) myChip8.setKeys(chip8key, true);
                    break;

                case SDL_QUIT:
                    quit = true;
                    break;

                default:
                    break;
            }
        }

        if (now - last >= STEP) {
            myChip8.emulateCycle();
            last += STEP;

            if (myChip8.drawFlag()) drawGraphics();
        }
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}