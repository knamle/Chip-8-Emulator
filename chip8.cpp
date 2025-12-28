#include "chip8.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib> 
#include <ctime> 

void chip8::initialize() {
    srand(time(0));

    pc     = 0x200;  // Program counter starts at 0x200
    opcode = 0;      // Reset current opcode	
    I      = 0;      // Reset index register
    sp     = 0;      // Reset stack pointer
    
    //Clear display	
    clearScreen();
    
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
    table = {
        &chip8::op00XX,
        &chip8::op1NNN,
    }; // 0xxx - Fxxx
}

void chip8::emulateCycle() {
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;

    // Decode Opcode
    uint8_t opMSB_4bits = opcode >> 12;
    
    // Execute Opcode
    Fn fn = table[opMSB_4bits];
    (this->*fn)(opcode);

    // Update timers
    if(delay_timer > 0) --delay_timer;
    
    if(sound_timer > 0) {
        if(sound_timer == 1) printf("BEEP!\n");
        --sound_timer;
    } 
}

void chip8::loadGame(std::string s) {
    try {
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
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}

void chip8::setKeys() {}

bool chip8::drawFlag() {
    return needToRedraw;
}

void chip8::clearScreen() {
    chip8::gfx.fill(0);
    needToRedraw = true;
}

void chip8::op00XX (uint16_t op){
    uint8_t lsbyte = op & 0x00ff;

    switch (lsbyte) {
        case 0xe0:
            clearScreen();
            break;
            
        case 0xee:
            sp--;
            pc = stack[sp];
            break;
    }
}

void chip8::op1NNN (uint16_t op) {
    uint16_t nnn = op & 0x0fff;
    pc = nnn;
}

void chip8::op2NNN (uint16_t op) {
    stack[sp] = pc;
    sp++;

    uint16_t nnn = op & 0x0fff;
    pc = nnn;
}

void chip8::op3XNN (uint16_t op) {
    uint8_t X = (op & 0x0f00) >> 8;
    uint8_t NN = op & 0xff;

    if (V[X] == NN) pc += 2;
}

void chip8::op4XNN (uint16_t op) {
    uint8_t X = (op & 0x0f00) >> 8;
    uint8_t NN = op & 0xff;

    if (V[X] != NN) pc += 2;
}

void chip8::op5XY0 (uint16_t op) {
    uint8_t X = (op & 0x0f00) >> 8;
    uint8_t Y = (op & 0x00f0) >> 4;

    if (V[X] == V[Y]) pc += 2;
}

void chip8::op6XNN (uint16_t op) {
    uint8_t NN = op & 0xff;
    uint8_t X = (op & 0x0f00) >> 8;

    V[X] = NN;
}

void chip8::op7XNN(uint16_t op) {
    const uint8_t x  = (op >> 8) & 0x0F;
    const uint8_t nn = op & 0xFF;
    V[x] = (V[x] + nn); 
}

void chip8::op8XY0(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;
    V[x] = V[y];
}

void chip8::op8XY1(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;
    V[x] |= V[y];
}

void chip8::op8XY2(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;
    V[x] &= V[y];
}

void chip8::op8XY3(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;
    V[x] ^= V[y];
}

void chip8::op8XY4(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;

    const uint16_t checkForOverflow = (V[x] + V[y]) & 0xff00;
    
    V[x] += V[y];

    checkForOverflow ? V[0xf] = 1 : V[0xf] = 0;
}

void chip8::op8XY5(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;

    const bool checkForUnderflow = V[x] < V[y];
    
    V[x] -= V[y];

    checkForUnderflow ? V[0xf] = 0 : V[0xf] = 1;
}

void chip8::op8XY6(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;

    V[0xf] = V[x] & 0x1;
    V[x] >>= 1;
}

void chip8::op8XY7(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;

    V[0xf] = (V[y] >= V[x]);
    V[x] = V[y] - V[x];
}

void chip8::op8XYE(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;

    V[0xf] = (V[x] >> 7);
    V[x] <<= 1;
}

void chip8::op9XY0(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;

    if (V[x] != V[y]) pc += 2;
}

void chip8::opANNN(uint16_t op) {
    I = op & 0x0FFF;
}

void chip8::opBNNN(uint16_t op) {
    pc = (op & 0x0FFF) + V[0];
}

void chip8::opCXNN(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
      
    uint8_t randomNum = static_cast<uint8_t>(rand() % 256);

    V[x] = (op & 0x0FF) & randomNum;
}

