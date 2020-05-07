#include "chip8.h"

void Chip8::initialize()
{
	// Initialize registers and memory once

	pc     = 0x200; // Program counter starts at 0x200
	opcode = 0;		// Reset current opcode
	I      = 0;		// Reset index register
	sp	   = 0;		// Reset stack pointer

	// Clear display
	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	for (int i = 0; i < 16; ++i)
		key[i] = V[i] = 0;
	
	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

	// Load fontset
	for (int i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;
}

void Chip8::loadGame(int bufferSize, char* buffer)
{
	for (int i = 0; i < bufferSize; i++)
		memory[i + 512] = buffer[i];
}

void Chip8::emulateCycle()
{
	// Fetch Opcode (2 bytes merged)
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode Opcode & execute
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
			case 0x0000: // 0x00E0 Clears the screen
				printf("Opcode: 0x%X\n", opcode);
				clearFlag = true;
				pc += 2;
				break;
			case 0x000E: // 0x00EE: Returns from subroutine
				sp = sp - 1;
				pc = stack[sp];
				pc += 2;
				break;
			default: 
				printf("Unkown opcode: 0x%X\n", opcode);
				break;
		}
		break;
	case 0x1000: // 1NNN: Jump to address NNN
		pc = opcode & 0x0FFF;
		break;
	case 0x2000: // Call subroutine at address NNN
		stack[sp] = pc + 2;
		sp++;
		pc = opcode & 0x0FFF;
		break;
	case 0x3000: // 3XNN - Skip next instruction if VX = NN
		pc += V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF) ? 4 : 2;
		break;
	case 0x4000: // 4XNN - Skip next instruction if VX != NN
		pc += V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF) ? 4 : 2;
		break;
	case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
		pc += V[opcode & 0x0F00 >> 8] == V[opcode & 0x00F0 >> 4] ? 4 : 2;
		break;
	case 0x6000: // 6XNN - Sets VX to NN
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2;
		break;
	case 0x7000: // 7XNN - Adds NN to VX (Carry flag not changed)
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // 8XY0 - Sets VX to VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0001: // 8XY1 - Set VX to VX OR VY
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0002: // 8XY2 - Set VX to VX AND VY
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0003: // 8XY3 - Set VX to VX XOR VY
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0004: // 8XY4 Add value of VY to VX, Register VF/Carry is set to 1 when value > 255
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; // carry
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0005: // 8XY5 - VY is subtracted from VX. VF set to 0 when there's a borrow, 1 if there isn't
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 0; // carry/borrow
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0006: // 8XY6 - Store least significant bit of VX in VF and shift VX to right by 1
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[opcode & 0x0F00 >> 8] >>= 1;
			pc += 2;
			break;
		case 0x0007: // 8XY7 - Set VX to VY minus VX. VF set to 0 when there's a brrow, 1 if there isn't
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
				V[0xF] = 0; // carry/borrow
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x000E: // 8XYE - Store most significant bit of VX in VF and shift VX to left by 1
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			pc += 2;
			break;
		default:
			printf("Unkown opcode: 0x%X\n", opcode);
			break;
		}
		break;
	case 0x9000: // 0x9XY0: Skips the next instruction if VX doesn't equal VY
		pc += (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
	case 0xA000: // ANNN: Sets I to the adress NNN
		I = opcode & 0x0FFF;
		pc += 2;
		break;
	case 0xB000: // BNNN: Jumps to the address NNN plus V0
		pc = (opcode & 0x0FFF) + V[0];
		break;
	case 0xC000: // CXNN - Sets VX to result of bitwise and on a random number and NN
		V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x0FF);
		pc += 2;
		break;
	case 0xD000:
	{
		unsigned short vx = V[(opcode & 0x0F00) >> 8];
		unsigned short vy = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;

		for (int y = 0; y < height; y++)
		{
			pixel = memory[I + y];
			for (int x = 0; x < 8; x++)
			{
				if ((pixel & (0x80 >> x)) != 0)
				{
					if (gfx[vx + x + (vy + y) * 64] == 1)
						V[0xF] = 1; // collision must've happened
					gfx[vx + x + (vy + y) * 64] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
		break;
	}
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: // EX9E - if the key stored in VX is pressed skip next instruction
			if (key[V[(opcode & 0x0F00) >> 8]] == true)
				pc += 4;
			else
				pc += 2;
			break;
		case 0x00A1: // EXA1 - if the key stored in VX is released skip next instruction
			if (key[V[(opcode & 0x0F00) >> 8]] == false)
				pc += 4;
			else
				pc += 2;
			break;
			break;
		default:
			printf("Unkown opcode: 0x%X\n", opcode);
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // FX07 - Set VX to value of delayTimer
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;
		case 0x000A: // FX0A - Waits for key to be pressed, stores it in VX
		{
			bool keyPress = false;

			for (int i = 0; i < 16; i++)
			{
				if (key[i] == true)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					keyPress = true;
				}
			}
			if (!keyPress)
				return;
			pc += 2;
			break;
		}
		case 0x0015: // FX15 - Set delay timer to value of VX
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x0018: // FX18 - Set the sound timer to VX
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x001E: // FX1E: Adds VX to I
			if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
				V[0xF] = 1;
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x0029: // FX29 - Set I to location of sprite for character in VX
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;
		case 0x0033: // Store bin-coded decimal representation of VX at addresses I, I+1, & I+2
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
			break;
		case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
				memory[I + i] = V[i];
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;
		case 0x0065: // FX65 - Fill V0 to VX with memory starting at address I 
			for (int i = 0; i < (opcode & (0x0F00) >> 8); i++)
			{
				V[i] = memory[I + i];
			}
			pc += 2;
			break;
		default:
			printf("Unkown opcode: 0x%X\n", opcode);
			break;
		}
		break;
	default:
		printf("Unkown opcode: 0x%X\n", opcode);
	}

	// Update timers
	if (delay_timer > 0)
		delay_timer--;
	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
		sound_timer--;
	}
}

void Chip8::setKeys()
{
}
