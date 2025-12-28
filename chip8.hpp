#include <string>
#include <array>
class chip8 {
    public:
        void initialize();
        void loadGame(std::string s);
        void emulateCycle();
        void setKeys();
        bool drawFlag();
        
        inline static std::array<uint8_t, 64 * 32> gfx;
        
    private:
        void clearScreen ();
        void op00XX (uint16_t op);
        void op1NNN (uint16_t op);
        void op2NNN (uint16_t op);
        void op3XNN (uint16_t op);
        void op4XNN (uint16_t op);
        void op5XY0 (uint16_t op);
        void op6XNN (uint16_t op);
        void op7XNN(uint16_t op);
        void op8XY0(uint16_t op);
        void op8XY1(uint16_t op);
        void op8XY2(uint16_t op);
        void op8XY3(uint16_t op);
        void op8XY4(uint16_t op);
        void op8XY5(uint16_t op);
        void op8XY6(uint16_t op);
        void op8XY7(uint16_t op);
        void op8XYE(uint16_t op);
        void op9XY0(uint16_t op);
        void opANNN(uint16_t op); 
        void opBNNN(uint16_t op); 
        void opCXNN(uint16_t op); 

        bool needToRedraw = false;
    
        std::array<uint8_t, 4096> memory;
        std::array<uint8_t, 16> V;
        std::array<uint8_t, 16> key;
        std::array<uint16_t, 16> stack;

        using Fn = void(chip8::*)(uint16_t op);

        std::array<Fn, 16> table{};

        uint8_t delay_timer;
        uint8_t sound_timer;
        
        uint16_t opcode; // current opcode
        uint16_t I;  // index from 0x000 to 0xFFF
        uint16_t pc; // prog counter from 0x000 to 0xFFF
        uint16_t sp; 

        /*
        0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
        0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
        0x200-0xFFF - Program ROM and work RAM
        */
        static constexpr int fontOffset = 0x50;
        static constexpr int programOffset = 0x200;

        std::array<uint8_t, 80> chip8_fontset =
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
};
