//#include   // OpenGL graphics and input
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "chip8.hpp"

chip8 myChip8;

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;
constexpr int CHIP8_W = 64;
constexpr int CHIP8_H = 32;

void setupGraphics() {
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " 
                    << SDL_GetError()
                        << std::endl;
    } else {
        //Create window
        window = SDL_CreateWindow( "nomnomnam", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL ) {
            std::cerr << "Window could not be created! SDL_Error: " 
                        << SDL_GetError()
                         << std::endl;
        } else {
            SDL_Renderer* renderer = SDL_CreateRenderer(
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
            
            //Hack to get window to stay up 
            SDL_Event e; 
            bool quit = false; 
            while( quit == false ){ 
                while( SDL_PollEvent( &e ) ){ 
                    if( e.type == SDL_QUIT ) quit = true; 

                    // Clear to black
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);

                    // Draw white pixel at (10,10) as an example
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawPoint(renderer, 10, 10);

                    SDL_RenderPresent(renderer);
                } 
            }
        }
    }

    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();
}

void drawGraphics() {
    for (int i=0; i<32; ++i) {
        for (int j=0; j<64; ++j) {
            if (chip8::gfx[j * 32 + i]) {
                return;
            }
        }
  }
}

int main (int argc, char **argv) {
    setupGraphics();
    //setupInput();

    myChip8.initialize();
    myChip8.loadGame("pong");

    for(;;) {
        myChip8.emulateCycle();

        if (myChip8.drawFlag()) drawGraphics();

        myChip8.setKeys();
    }
    
    return 0;
}