//#include   // OpenGL graphics and input
#include <SDL.h>
#include <stdio.h>
#include <iostream>
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
}

int main (int argc, char **argv) {
    setupGraphics();
    //setupInput();

    myChip8.initialize();

    myChip8.loadGame("pong");

    SDL_Event e; 
    bool quit = false; 
    int i = 0;

    while( quit == false ){ 
        while( SDL_PollEvent( &e ) ){ 
            if( e.type == SDL_QUIT ) quit = true; 

            myChip8.emulateCycle();

            chip8::gfx[i++] = 1;
            chip8::gfx[1000] = 1;

            if (myChip8.drawFlag()) drawGraphics();

            myChip8.setKeys();
        } 
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}