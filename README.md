# Chip-8-Emulator

# TODO check the commands are legit for both os's
mac:
brew install sdl2 pkg-config

ubuntu:
sudo apt update
sudo apt install build-essential cmake pkg-config libsdl2-dev

cmake -S . -B build
cmake --build build -j
./build/main

The goal of this project was to implement a Chip-8 emulator allowing for roms to be run and played.
To start we need to install SDL2 and pkg-config:

git clone git@github.com:knamle/Chip-8-Emulator.git

On macOS:

```brew install sdl2 pkg-config```

On 


### Tutorials
https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
https://multigesture.net/articleshow-to-write-an-emulator-chip-8-interpreter/

### Source for game and testing ROMs
https://github.com/kripod/chip8-roms/tree/master
https://github.com/Timendus/chip8-test-suite/tree/main
