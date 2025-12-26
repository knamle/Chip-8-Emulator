#include "chip8.hpp"

void chip8::initialize() {
    pc     = 0x200;  // Program counter starts at 0x200
    opcode = 0;      // Reset current opcode	
    I      = 0;      // Reset index register
    sp     = 0;      // Reset stack pointer
    
    // TODO: Clear display	
    
    // Clear stack
    // Clear registers V0-VF
    stack.fill(0);
    V.fill(0);

    // Clear memory
    memory.fill(0);
    
    // Load fontset
    for(int i = 0; i < 80; ++i) memory[i] = chip8_fontset[i];		
    
    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    // init function pointers
    using Fn = void(*)();
    Fn table[16]; // 0xxx - Fxxx
}

void chip8::emulateCycle() {
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1]; // where does pc get updated?

    // Decode Opcode
    
    // Execute Opcode

    // Update timers
}

void chip8::loadGame(std::string s) {};

void chip8::setKeys() {};

int  chip8::drawFlag() {};