//#include   // OpenGL graphics and input

#include "chip8.hpp"

chip8 myChip8;

int main (int argc, char **argv) {
    //setupGraphics();
    //setupInput();

    myChip8.initialize();
    myChip8.loadGame("pong");

    for(;;) {
        myChip8.emulateCycle();

        if (myChip8.drawFlag()) //drawGraphics();

        myChip8.setKeys();
    }
    
    return 0;
}