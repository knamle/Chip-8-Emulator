cmake_minimum_required(VERSION 3.20)
project(Chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(chip8_lib chip8.cpp)
target_include_directories(chip8_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE chip8_lib)

# (macOS/homebrew + Linux)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
target_link_libraries(main PRIVATE PkgConfig::SDL2)

# Warnings
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)

# Sanitizers
option(ENABLE_SANITIZERS "Enable AddressSanitizer + UBSanitizer" OFF)
if(ENABLE_SANITIZERS)
  target_compile_options(main PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g -O1)
  target_link_options(main PRIVATE -fsanitize=address,undefined)
endif()
