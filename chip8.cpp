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
    // Clear keys
    stack.fill(0);
    V.fill(0);
    key.fill(0);

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
        &chip8::op2NNN, 
        &chip8::op3XNN, 
        &chip8::op4XNN, 
        &chip8::op5XY0, 
        &chip8::op6XNN, 
        &chip8::op7XNN, 
        &chip8::op8XYN, 
        &chip8::op9XY0, 
        &chip8::opANNN, 
        &chip8::opBNNN, 
        &chip8::opCXNN, 
        &chip8::opDXYN, 
        &chip8::opEXNN, 
        &chip8::opFXNN  
    }; // 0xxx - Fxxx
}

void chip8::emulateCycle() {
    // Update timers
    if(delay_timer > 0) --delay_timer;
    
    if(sound_timer > 0) {
        if(sound_timer == 1) printf("BEEP!\n");
        --sound_timer;
    } 

    if (waitingForKey) return;

    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;

    // Decode Opcode
    uint8_t opMSB_4bits = opcode >> 12;
    std::cout << std::hex << "opcode:" << (opcode) << "\n"; 
 
    // Execute Opcode
    Fn fn = table[opMSB_4bits];
    (this->*fn)(opcode);
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

void chip8::setKeys(uint8_t chip8key, bool isDown) {
    if (chip8key == 0xff) return;

    std::cout << "chip8key=" << std::hex << int(chip8key)
          << " keyUp=" << isDown
          << " waiting=" << waitingForKey << "\n";
    
    if (waitingForKey && isDown) {
        V[waitingReg] = chip8key;
        waitingForKey = false;
    }
    
    isDown ? key[chip8key] = 1 : key[chip8key] = 0;
}

bool chip8::drawFlag() {
    return needToRedraw;
}

void chip8::setDrawFlag(bool redraw) {
    needToRedraw = redraw;
}

void chip8::clearScreen() {
    chip8::gfx.fill(0);
    setDrawFlag(true);
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

void chip8::op8XYN(uint16_t op) {
    const uint8_t x = (op >> 8) & 0x0F;
    const uint8_t y = (op >> 4) & 0x0F;

    switch (op & 0xF) {
        case 0x0: 
            V[x] = V[y];
            break;

        case 0x1: 
            V[x] |= V[y];
            break;

        case 0x2: 
            V[x] &= V[y];
            break;

        case 0x3: 
            V[x] ^= V[y];
            break;

        case 0x4: { 
            const uint16_t checkForOverflow = (V[x] + V[y]) & 0xff00;
    
            checkForOverflow ? V[0xf] = 1 : V[0xf] = 0;
            V[x] += V[y];
            break;
        }

        case 0x5: {
            const bool checkForUnderflow = V[x] < V[y];
            checkForUnderflow ? V[0xf] = 0 : V[0xf] = 1;
    
            V[x] -= V[y];
            break;
        }

        case 0x6: {
            V[0xF] = V[x] & 0x1;
            V[x] >>= 1;
            break;
        }

        case 0x7: {
            V[0xF] = (V[y] >= V[x]);
            V[x]   = V[y] - V[x];
            break;
        }

        case 0xE: {
            V[0xF] = (V[x] >> 7) & 0x01;
            V[x] <<= 1;
            break;
        }

        default: {
            break;
        }
    }
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

void chip8::opDXYN(uint16_t op) {
    uint8_t x = V[(opcode & 0x0F00) >> 8];
    uint8_t y = V[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;
    uint8_t pixel;

    V[0xf] = 0;
    for (int yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        
        for(int xline = 0; xline < 8; xline++) {
            if((pixel & (0x80 >> xline)) != 0) {
                if(gfx[(x + xline + ((y + yline) * 64))] == 1) V[0xF] = 1;                                 
                
                gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }
    setDrawFlag(true);
}

void chip8::opEXNN(uint16_t op) {
    uint8_t x = (op >> 8) & 0x0F;
    uint8_t nn = op & 0xff;

    uint8_t keyIndex = V[x] & 0xf;

    switch(nn) {
        case 0x9e:
            if (key[keyIndex] != 0) pc += 2;
            break;

        case 0xa1:
            if (key[keyIndex] == 0) pc += 2;
            break;
    }
}

void chip8::opFXNN(uint16_t op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t nn = op & 0xff;

    switch (nn) {
        case 0x07:
            V[x] = delay_timer;
            break;

        case 0x0a:
            waitingForKey = true;
            waitingReg = x;
            break;

        case 0x15: 
            delay_timer = V[x];
            break;

        case 0x18: 
            sound_timer = V[x];
            break;

        case 0x1E: {
            I += V[x];
            break;
        }

        case 0x29: {
            I = (op & 0xf) * 5 + fontOffset;
            break;
        }

        case 0x33: {
            memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
            break;
        }

        case 0x55: {
            for (uint8_t j = 0; j <= x; ++j) {
                memory[I + j] = V[j]; 
            }
            I = I + x + 1;
            break;
        }

        case 0x65: {
            for (uint8_t j = 0; j <= x; ++j) {
                V[j] = memory[I + j];
            }
            I = I + x + 1;
            
            break;
        }

    }

}