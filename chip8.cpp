#include "chip8.hpp"
#include <fstream>
#include <vector>
#include <iostream>

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
    for(int i = 0; i < 80; ++i) memory[fontOffset + i] = chip8_fontset[i];		
    
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
    if(delay_timer > 0) --delay_timer;
    
    if(sound_timer > 0) {
        if(sound_timer == 1) printf("BEEP!\n");
        --sound_timer;
    } 
}

void chip8::loadGame(std::string s) {
    std::ifstream stream(s, std::ios::binary | std::ios::ate);
    const int programSize = stream.tellg();
    stream.seekg(0);

    if (programSize > 3584) {
        std::cout << "Program too big bro, try a different one";
        return;
    }

    std::vector<char> buffer(programSize);

    stream.read(buffer.data(), programSize);

    for(int i = 0; i < programSize; ++i) memory[i + programOffset] = buffer[i];
}

void chip8::setKeys() {}

int  chip8::drawFlag() {
    return 0;
}