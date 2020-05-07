#pragma once

#include <stdio.h>
#include <array>

/*
 *  [ System Memory ]------------------------------------------
 *	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 *	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 *	0x200-0xFFF - Program ROM and work RAM
 *  -----------------------------------------------------------
 */

 /*
  *  [ Graphics System ]----------------------------------------
  *	- Drawing done in XOR mode, if pixel is turned off as result,
  *   collision has been made.
  * - Graphics are black and white, screen has 2048 pixels,
  *   (64 x 32)
  *  -----------------------------------------------------------
  */

class Chip8
{
public:

    void initialize();                                   // Initialize the chip-8

    void loadGame(int bufferSize, char* buffer);         // Load a game into memory

    void emulateCycle();                                 // Emulate one cycle

    void setKeys();                                      // Store key press state

    bool drawFlag;                                       // if set, screen should be updated
    bool clearFlag;                                      // if set, screen should be cleared

    // HEX based keypad (0x0 - 0xF)
    bool key[16];

    std::array <bool, 64 * 32> gfx = { false };          // pixels (holds state of 1 or 0)

private:
    // two byte operation code
    unsigned short opcode;

    // 4K memory
    unsigned char memory[4096];

    // 16 Registers V0, V1... VE
    unsigned char V[16];

    // index register
    unsigned short I;

    // program counter
    unsigned short pc;

    // timer registers (count down at 60Hz)
    unsigned char delay_timer;
    // When reached to zero, buzzer sounds
    unsigned char sound_timer;

    // stack
    unsigned short stack[16];
    // stack pointer
    unsigned short sp;

    // Each number or character is 4 pixels wide and 5 pixel high
    unsigned char chip8_fontset[80] =
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

