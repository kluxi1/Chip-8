#include <gtest/gtest.h>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <thread>

#include "../chip8.h"

class Chip8Test : public ::testing::Test {
	protected:
	 	void SetUp() override {cpu.init();}

		//void TearDown() override {}
	
		Chip8 cpu;

};

void runInstruction(Chip8& cpu, const uint16_t& opcode){
	
	cpu.opcode = opcode;
	cpu.pcInc();
	cpu.exec();
}

TEST_F(Chip8Test, init) {

	uint8_t fontData[80] {0xF0, 0x90, 0x90, 0x90, 0xF0,	// '0'
			      0x20, 0x60, 0x20, 0x20, 0x70,	// '1'
			      0xF0, 0x10, 0xF0, 0x80, 0xF0,	// '2'
			      0xF0, 0x10, 0xF0, 0x10, 0xF0,	// '3'
			      0x90, 0x90, 0xF0, 0x10, 0x10,	// '4'
			      0xF0, 0x80, 0xF0, 0x10, 0xF0,	// '5'
			      0xF0, 0x80, 0xF0, 0x90, 0xF0,	// '6'
			      0xF0, 0x10, 0x20, 0x40, 0x40,	// '7'
			      0xF0, 0x90, 0xF0, 0x90, 0xF0,	// '8'
			      0xF0, 0x90, 0xF0, 0x10, 0xF0,	// '9'
			      0xF0, 0x90, 0xF0, 0x90, 0x90,	// 'A'
			      0xE0, 0x90, 0xE0, 0x90, 0xE0,	// 'B'
			      0xF0, 0x80, 0x80, 0x80, 0xF0,	// 'C'
			      0xE0, 0x90, 0x90, 0x90, 0xE0,	// 'D'
			      0xF0, 0x80, 0xF0, 0x80, 0xF0,	// 'E'	      	
			      0xF0, 0x80, 0xF0, 0x80, 0x80	// 'F'
			      };


	ASSERT_EQ(cpu.pc, 0x200);	//program counter value
	
	
	//memory {0x00 - 0x4F} should be all 0x0
	for(unsigned int it = 0x0;  it != 0x50; ++it) {
	
		EXPECT_EQ(cpu.memory[it], 0x0);
	
	}
	
	//verifing if the font is loaded on address {0x50 - 0x9F} 
	for(unsigned int it = 0;  it != 80; ++it) {
	
		ASSERT_EQ(cpu.memory[0x50 + it], fontData[it]);
	
	}
	
	//memory {0xA0 - 0x1FF} should be all 0x0
	for(unsigned int it = 0xA0;  it != 0x200; ++it) {
	
		EXPECT_EQ(cpu.memory[it], 0x0);

	}
	
}

TEST_F(Chip8Test, loadRom) {

	cpu.loadRom("pong.ch8");
		
	//check rom loaded
	ASSERT_EQ(cpu.memory[0x200 + 0], 0x6A);
	ASSERT_EQ(cpu.memory[0x200 + 165], 0x18);
	ASSERT_EQ(cpu.memory[0x200 + 72], 0x40);
	ASSERT_EQ(cpu.memory[0x200 + 36], 0x69);



}

TEST_F(Chip8Test, loadRomNotFound) {

	//give error if rom file don't exist

	EXPECT_THROW(cpu.loadRom(""), std::runtime_error);

}

TEST_F(Chip8Test, loadRomTooBig) {

	//give error if rom is too big

	EXPECT_THROW(cpu.loadRom("big.ch8"), std::runtime_error);

}

TEST_F(Chip8Test, getOpcode) {
	
	uint8_t testRom[6] = {0x00,0xE0, //CLS
			      0x00,0xEE, //RET
			      0x81, 0x20 //LD V1, V2
			      };
	
	
	//cpu.loadRom(testRom)
		for(unsigned int FontDataIterator = 0; FontDataIterator != std::end(testRom) - std::begin(testRom) ; ++FontDataIterator) {
			
			cpu.memory[0x200 + FontDataIterator] = testRom[FontDataIterator];
		
		}
		
	
	cpu.getOpcode();
	ASSERT_EQ(cpu.opcode, 0x00E0);
	cpu.pcInc();
	ASSERT_EQ(cpu.pc, 0x202);
	
	cpu.getOpcode();
	ASSERT_EQ(cpu.opcode, 0x00EE);
	cpu.pcInc();
	ASSERT_EQ(cpu.pc, 0x204);
	
	cpu.getOpcode();
	ASSERT_EQ(cpu.opcode, 0x8120);
	cpu.pcInc();
	ASSERT_EQ(cpu.pc, 0x206);
	
	//almost outside ram
	cpu.pc = 0xFFE;
	cpu.memory[0xFFE] =0x81; 
	cpu.memory[0xFFF] = 0x20;
	cpu.getOpcode();
	ASSERT_EQ(cpu.opcode, 0x8120);
	//exec();	wont change pc
	EXPECT_THROW(cpu.pcInc(), std::runtime_error);
	
}

TEST_F(Chip8Test, CLS) {
	
	//set screen data to 1
	for (unsigned int y = 0; y != 32; ++y) {
		
		for (unsigned int x = 0; x != std::end(cpu.display) - std::begin(cpu.display); ++x) {
			cpu.display[x][y] = true;
		}
	}
		
	runInstruction(cpu, 0x00E0);
	
	//checking if the execution succeeded	
	for (unsigned int y = 0; y != 32; ++y) {
		
		for (unsigned int x = 0; x != std::end(cpu.display) - std::begin(cpu.display); ++x) {
			ASSERT_EQ(cpu.display[x][y], false);
		}
	}
}


TEST_F(Chip8Test, RET) {

	cpu.usingStack = true;
	cpu.pc = 0x670;
	cpu.sp = 0x2;
	cpu.stack[0x0] = 0x200;
	cpu.stack[0x1] = 0x300;	
	cpu.stack[0x2] = 0x400;
		
	runInstruction(cpu, 0x00EE);
	
	ASSERT_EQ(cpu.pc, 0x400);
	ASSERT_EQ(cpu.sp, 0x1);
	ASSERT_EQ(cpu.usingStack, true);
	
	runInstruction(cpu, 0x00EE);
	
	ASSERT_EQ(cpu.pc, 0x300);
	ASSERT_EQ(cpu.sp, 0x0);
	ASSERT_EQ(cpu.usingStack, true);
	
	runInstruction(cpu, 0x00EE);
	
	ASSERT_EQ(cpu.pc, 0x200);
	ASSERT_EQ(cpu.sp, 0x0);
	ASSERT_EQ(cpu.usingStack, false);
	
	EXPECT_THROW(runInstruction(cpu, 0x00EE), std::runtime_error);	

}

TEST_F(Chip8Test, JPaddr) {
	
	runInstruction(cpu, 0x1500);
	ASSERT_EQ(cpu.pc, 0x500);
	
	runInstruction(cpu, 0x1786);
	ASSERT_EQ(cpu.pc, 0x786);
	
	runInstruction(cpu, 0x1FFF);
	ASSERT_EQ(cpu.pc, 0xFFF);
	
}

TEST_F(Chip8Test, CALLaddr) {

	ASSERT_EQ(cpu.usingStack, false);
	
	runInstruction(cpu, 0x2300);
	
	ASSERT_EQ(cpu.pc, 0x300);
	ASSERT_EQ(cpu.sp, 0x0);
	ASSERT_EQ(cpu.stack[0x0], 0x202);
	ASSERT_EQ(cpu.usingStack, true);
	
	runInstruction(cpu, 0x2400);
	
	ASSERT_EQ(cpu.pc, 0x400);
	ASSERT_EQ(cpu.sp, 0x1);
	ASSERT_EQ(cpu.stack[0x1], 0x302);
	ASSERT_EQ(cpu.usingStack, true);
	
	runInstruction(cpu, 0x2500);
	
	ASSERT_EQ(cpu.pc, 0x500);
	ASSERT_EQ(cpu.sp, 0x2);
	ASSERT_EQ(cpu.stack[0x2], 0x402);
	ASSERT_EQ(cpu.usingStack, true);
	
	//stack overflow test
	
	for (unsigned it = 0; it != 13; it++) {
		runInstruction(cpu, 0x2600 + it);
	}
	
	EXPECT_THROW(runInstruction(cpu, 0x2700), std::runtime_error);
	
}

TEST_F(Chip8Test, SE_Vx_byte) {
	/*3xkk - SE Vx, byte
Skip next instruction if Vx = kk.

The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
*/

	
	// v0 = 0x0, kk = 0x55
	runInstruction(cpu, 0x3055);
	EXPECT_EQ(cpu.pc, 0x202);
	
	// v5 = 0x69, kk = 0x69
	cpu.v[5] = 0x69;
	runInstruction(cpu, 0x3569);
	EXPECT_EQ(cpu.pc, 0x206);
	
	// v8 = 0x45, kk = 0x45
	// outside memory error
	cpu.pc = 0xFFC;
	cpu.v[8] = 0x45;
	EXPECT_THROW(runInstruction(cpu, 0x3845), std::runtime_error);
}

TEST_F(Chip8Test, SNE_Vx_byte) {
/*4xkk - SNE Vx, byte
Skip next instruction if Vx != kk.
*/
	
	
	// v0 = 0x0, kk = 0x55
	runInstruction(cpu, 0x4055);
	EXPECT_EQ(cpu.pc, 0x204);
	
	// v5 = 0x69, kk = 0x69
	cpu.v[5] = 0x69;
	runInstruction(cpu, 0x4569);
	EXPECT_EQ(cpu.pc, 0x206);
	
	// v8 = 0x45, kk = 0x45
	// outside memory error
	cpu.pc = 0xFFC;
	cpu.v[8] = 0x40;
	EXPECT_THROW(runInstruction(cpu, 0x4845), std::runtime_error);


}

TEST_F(Chip8Test, SE_Vx_Vy) {
/*5xy0 - Skip next instruction if Vx = Vy.
The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
*/

	// v0 = 0x0, v7 = 0x55
	cpu.v[7] = 0x55;
	runInstruction(cpu, 0x5070);
	EXPECT_EQ(cpu.pc, 0x202);
	
	// v5 = 0x69, v3 = 0x69
	cpu.v[5] = 0x69;
	cpu.v[3] = 0x69;
	runInstruction(cpu, 0x5530);
	EXPECT_EQ(cpu.pc, 0x206);
	
	// v8 = 0x45, v1 = 0x45
	// outside memory error
	cpu.pc = 0xFFC;
	cpu.v[8] = 0x45;
	cpu.v[1] = 0x45;
	EXPECT_THROW(runInstruction(cpu, 0x5810), std::runtime_error);
}

TEST_F(Chip8Test, LD_Vx_byte) {
/*6xkk - Set Vx = kk.

The interpreter puts the value kk into register Vx.
*/

	runInstruction(cpu, 0x6434);
	EXPECT_EQ(cpu.v[4], 0x34);
	
	runInstruction(cpu, 0x6026);
	EXPECT_EQ(cpu.v[0], 0x26);
	
	runInstruction(cpu, 0x6AFF);
	EXPECT_EQ(cpu.v[10], 0xFF);

}

TEST_F(Chip8Test,  ADD_Vx_byte) {
/*7xkk - Set Vx = Vx + kk.

Adds the value kk to the value of register Vx, then stores the result in Vx. 
*/

	runInstruction(cpu, 0x7012);
	EXPECT_EQ(cpu.v[0], 0x12);
	
	runInstruction(cpu, 0x7069);
	EXPECT_EQ(cpu.v[0], 0x7B);
	
	runInstruction(cpu, 0x70FF);
	EXPECT_EQ(cpu.v[0], 0x7A);
	
	
	cpu.v[8] = 0x1B;
	runInstruction(cpu, 0x787A);
	EXPECT_EQ(cpu.v[8], 0x95);

}

TEST_F(Chip8Test, LD_Vx_Vy) {
/*8xy0 - Set Vx = Vy.

Stores the value of register Vy in register Vx.
*/
	cpu.v[12] = 0xAB; //Vy
	runInstruction(cpu, 0x80C0);
	EXPECT_EQ(cpu.v[0], 0xAB);
	
	cpu.v[6] = 0xFF; //Vy
	runInstruction(cpu, 0x8060);
	EXPECT_EQ(cpu.v[0], 0xFF);
	
	cpu.v[7] = 0x5; //Vy
	runInstruction(cpu, 0x8370);
	EXPECT_EQ(cpu.v[3], 0x5);

}

TEST_F(Chip8Test, OR_Vx_Vy) {
/*8xy1 - Set Vx = Vx OR Vy.

Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0. 
*/

	cpu.v[0] =  0b00001111; //Vx
	cpu.v[10] = 0b11110000; //Vy
	runInstruction(cpu, 0x80A1);
	EXPECT_EQ(cpu.v[0], 0b11111111);
	EXPECT_EQ(cpu.v[10], 0b11110000);
	
	cpu.v[2] = 0b01010101; //Vx
	cpu.v[8] = 0b11100110; //Vy
	runInstruction(cpu, 0x8281);
	EXPECT_EQ(cpu.v[2], 0b11110111);
	EXPECT_EQ(cpu.v[8], 0b11100110);
}

TEST_F(Chip8Test, AND_Vx_Vy) {
/*8xy2 - Set Vx = Vx AND Vy.

Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0. 
*/

	cpu.v[0] =  0b00001111; //Vx
	cpu.v[10] = 0b11110000; //Vy
	runInstruction(cpu, 0x80A2);
	EXPECT_EQ(cpu.v[0], 0b00000000);
	EXPECT_EQ(cpu.v[10], 0b11110000);
	
	cpu.v[2] = 0b01010101; //Vx
	cpu.v[8] = 0b11100110; //Vy
	runInstruction(cpu, 0x8282);
	EXPECT_EQ(cpu.v[2], 0b01000100);
	EXPECT_EQ(cpu.v[8], 0b11100110);
}

TEST_F(Chip8Test, XOR_Vx_Vy) {
/*8xy3 - Set Vx = Vx XOR Vy.

Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0. 
*/
	
	cpu.v[0] =  0b00001111; //Vx
	cpu.v[10] = 0b11110000; //Vy
	runInstruction(cpu, 0x80A3);
	EXPECT_EQ(cpu.v[0], 0b11111111);
	EXPECT_EQ(cpu.v[10], 0b11110000);
	
	cpu.v[2] = 0b01010101; //Vx
	cpu.v[8] = 0b11100110; //Vy
	runInstruction(cpu, 0x8283);
	EXPECT_EQ(cpu.v[2], 0b10110011);
	EXPECT_EQ(cpu.v[8], 0b11100110);

}

TEST_F(Chip8Test, ADD_Vx_Vy) {
/*8xy4 - Set Vx = Vx + Vy, set VF = carry.

The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
*/

	cpu.v[12] = 0xAB; //Vy
	runInstruction(cpu, 0x80C4);
	EXPECT_EQ(cpu.v[0], 0xAB);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	cpu.v[6] = 0xFF; //Vy
	runInstruction(cpu, 0x8064);
	EXPECT_EQ(cpu.v[0], 0xAA);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	cpu.v[7] = 0x5; //Vy
	runInstruction(cpu, 0x8374);
	EXPECT_EQ(cpu.v[3], 0x5);
	EXPECT_EQ(cpu.v[0xF], 0);

}

TEST_F(Chip8Test, SUB_Vx_Vy) {
/*8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.

If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
*/

	cpu.v[12] = 0xAB; //Vy
	runInstruction(cpu, 0x80C5);
	EXPECT_EQ(cpu.v[0], 0x55);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	cpu.v[6] = 0x34; //Vy
	runInstruction(cpu, 0x8065);
	EXPECT_EQ(cpu.v[0], 0x21);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	cpu.v[7] = 0x5; //Vy
	runInstruction(cpu, 0x8375);
	EXPECT_EQ(cpu.v[3], 0xFB);
	EXPECT_EQ(cpu.v[0xF], 0);

}

TEST_F(Chip8Test, SHR_Vx_Vy) {
/*8xy6 - Set Vx = Vx SHR 1.

If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
*/

	cpu.v[12] = 0x14; //Vx
	runInstruction(cpu, 0x8C06);
	EXPECT_EQ(cpu.v[12], 0xA);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	cpu.v[7] = 0x5; //Vx
	runInstruction(cpu, 0x87F6);
	EXPECT_EQ(cpu.v[7], 0x2);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	cpu.v[6] = 0x34; //Vx
	runInstruction(cpu, 0x8676);
	EXPECT_EQ(cpu.v[6], 0x1A);
	EXPECT_EQ(cpu.v[0xF], 0);
	
}

TEST_F(Chip8Test, SUBN_Vx_Vy) {
/*
8xy7 - SUBN Vx, Vy
Set Vx = Vy - Vx, set VF = NOT borrow.

If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
*/

	cpu.v[12] = 0xAB; //Vy
	runInstruction(cpu, 0x80C7);
	EXPECT_EQ(cpu.v[0], 0xAB);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	cpu.v[6] = 0x34; //Vy
	runInstruction(cpu, 0x8067);
	EXPECT_EQ(cpu.v[0], 0x77);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	cpu.v[7] = 0x5; //Vy
	cpu.v[3] = 0x2; //Vx
	runInstruction(cpu, 0x8377);
	EXPECT_EQ(cpu.v[3], 0x3);
	EXPECT_EQ(cpu.v[0xF], 1);


}

TEST_F(Chip8Test, SHL_Vx_Vy) {
/*
8xyE - SHL Vx {, Vy}
Set Vx = Vx SHL 1.

If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
*/
	
	cpu.v[12] = 0x14; //Vx
	runInstruction(cpu, 0x8C0E);
	EXPECT_EQ(cpu.v[12], 0x28);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	cpu.v[7] = 0x85; //Vx
	runInstruction(cpu, 0x87FE);
	EXPECT_EQ(cpu.v[7], 0xA);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	cpu.v[6] = 0x34; //Vx
	runInstruction(cpu, 0x867E);
	EXPECT_EQ(cpu.v[6], 0x68);
	EXPECT_EQ(cpu.v[0xF], 0);
	
}

TEST_F(Chip8Test, SNE_Vx_Vy) {
/*
9xy0 - SNE Vx, Vy
Skip next instruction if Vx != Vy.

The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
*/
	cpu.v[3] = 65; //vx
	cpu.v[8] = 65; //vy
	
	
	runInstruction(cpu, 0x9380);
	EXPECT_EQ(cpu.pc, 0x202);
	
	runInstruction(cpu, 0x93D0);
	EXPECT_EQ(cpu.pc, 0x206);


}

TEST_F(Chip8Test, LD_I_addr) {
/*
Annn - LD I, addr
Set I = nnn.

The value of register I is set to nnn.
*/

	runInstruction(cpu, 0xA380);
	EXPECT_EQ(cpu.i, 0x380);
	
	runInstruction(cpu, 0xAFFF);
	EXPECT_EQ(cpu.i, 0xFFF);
}

TEST_F(Chip8Test, JP_V0_addr) {
/*
Bnnn - JP V0, addr
Jump to location nnn + V0.

The program counter is set to nnn plus the value of V0.
*/
	
	runInstruction(cpu, 0xB380);
	EXPECT_EQ(cpu.pc, 0x380);
	
	cpu.v[0] = 0x45;
	runInstruction(cpu, 0xB365);
	EXPECT_EQ(cpu.pc, 0x3AA);
	
	cpu.v[0] = 0x02;
	runInstruction(cpu, 0xBFFD);
	EXPECT_EQ(cpu.pc, 0xFFF);
	
	cpu.v[0] = 0xFF;
	EXPECT_THROW(runInstruction(cpu, 0xBFFC), std::runtime_error);

}

TEST_F(Chip8Test, RND_Vx_byte) {
/*
Cxkk - RND Vx, byte
Set Vx = random byte AND kk.

The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
*/

	runInstruction(cpu, 0xC300);
	EXPECT_EQ(cpu.v[3], 0x00);
	
	uint8_t randomUniqueNumber[5]{0};
	unsigned int repeats = 5;
	bool isUnique = false;

	
	while(repeats != 0){
		//create 5 random numbers
		for(unsigned int it = 0; it != 5; ++it) {
			runInstruction(cpu, 0xC2FF);
			randomUniqueNumber[it] = cpu.v[2];
		}
		
		//check if numbers are all unique
		for (unsigned int it = 0; it != 5; ++it) {
			
			
			unsigned int temp = std::count (randomUniqueNumber, randomUniqueNumber+5, randomUniqueNumber[it]);
			if (temp != 1) break;
			else if (it == 4) isUnique = true;
		}
		
		if (isUnique) break;
		else --repeats;
	
	}
	
	EXPECT_EQ(isUnique, true);
}

TEST_F(Chip8Test, DRW_Vx_Vy_nibble) {
/*
Dxyn - DRW Vx, Vy, nibble
Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
*/
	
	/*
		v0 = 0 --> Vx
		v1 = 0 --> Vy
		
		v2 = 8 --> Vx
		v3 = 0 --> Vy
		
		v4 = 62 --> Vx
		v5 = 15 --> Vy
	
		v6 = 31 --> Vx
		v7 = 30 --> Vy
		
		v8 = 60 --> Vx
		v9 = 28 --> Vy
	*/
	
	cpu.v[0] = 0;
	cpu.v[1] = 0;
	cpu.v[2] = 8;
	cpu.v[3] = 0;
	cpu.v[4] = 62;
	cpu.v[5] = 15;
	cpu.v[6] = 31;
	cpu.v[7] = 30;
	cpu.v[8] = 60;
	cpu.v[9] = 28;
	
	
	//draw '0' on screen coordinate (0, 0)
	cpu.i = 0x50;
	runInstruction(cpu, 0xD015);

	EXPECT_EQ(cpu.v[0xF], 0);
	
	EXPECT_EQ(cpu.display[0][0], true);
	EXPECT_EQ(cpu.display[1][0], true);
	EXPECT_EQ(cpu.display[2][0], true);
	EXPECT_EQ(cpu.display[3][0], true);
	EXPECT_EQ(cpu.display[4][0], false);
	EXPECT_EQ(cpu.display[5][0], false);
	EXPECT_EQ(cpu.display[6][0], false);
	EXPECT_EQ(cpu.display[7][0], false);
	
	EXPECT_EQ(cpu.display[0][1], true);
	EXPECT_EQ(cpu.display[1][1], false);
	EXPECT_EQ(cpu.display[2][1], false);
	EXPECT_EQ(cpu.display[3][1], true);
	EXPECT_EQ(cpu.display[4][1], false);
	EXPECT_EQ(cpu.display[5][1], false);
	EXPECT_EQ(cpu.display[6][1], false);
	EXPECT_EQ(cpu.display[7][1], false);
	
	EXPECT_EQ(cpu.display[0][2], true);
	EXPECT_EQ(cpu.display[1][2], false);
	EXPECT_EQ(cpu.display[2][2], false);
	EXPECT_EQ(cpu.display[3][2], true);
	EXPECT_EQ(cpu.display[4][2], false);
	EXPECT_EQ(cpu.display[5][2], false);
	EXPECT_EQ(cpu.display[6][2], false);
	EXPECT_EQ(cpu.display[7][2], false);
	
	EXPECT_EQ(cpu.display[0][3], true);
	EXPECT_EQ(cpu.display[1][3], false);
	EXPECT_EQ(cpu.display[2][3], false);
	EXPECT_EQ(cpu.display[3][3], true);
	EXPECT_EQ(cpu.display[4][3], false);
	EXPECT_EQ(cpu.display[5][3], false);
	EXPECT_EQ(cpu.display[6][3], false);
	EXPECT_EQ(cpu.display[7][3], false);
	
	EXPECT_EQ(cpu.display[0][4], true);
	EXPECT_EQ(cpu.display[1][4], true);
	EXPECT_EQ(cpu.display[2][4], true);
	EXPECT_EQ(cpu.display[3][4], true);
	EXPECT_EQ(cpu.display[4][4], false);
	EXPECT_EQ(cpu.display[5][4], false);
	EXPECT_EQ(cpu.display[6][4], false);
	EXPECT_EQ(cpu.display[7][4], false);
	
	//draw 'F' on screen coordinate (8, 0)
	cpu.i = 0x9B;
	runInstruction(cpu, 0xD235);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	EXPECT_EQ(cpu.display[8][0], true);
	EXPECT_EQ(cpu.display[9][0], true);
	EXPECT_EQ(cpu.display[10][0], true);
	EXPECT_EQ(cpu.display[11][0], true);
	EXPECT_EQ(cpu.display[12][0], false);
	EXPECT_EQ(cpu.display[13][0], false);
	EXPECT_EQ(cpu.display[14][0], false);
	EXPECT_EQ(cpu.display[15][0], false);
	
	EXPECT_EQ(cpu.display[8][1], true);
	EXPECT_EQ(cpu.display[9][1], false);
	EXPECT_EQ(cpu.display[10][1], false);
	EXPECT_EQ(cpu.display[11][1], false);
	EXPECT_EQ(cpu.display[12][1], false);
	EXPECT_EQ(cpu.display[13][1], false);
	EXPECT_EQ(cpu.display[14][1], false);
	EXPECT_EQ(cpu.display[15][1], false);
	
	EXPECT_EQ(cpu.display[8][2], true);
	EXPECT_EQ(cpu.display[9][2], true);
	EXPECT_EQ(cpu.display[10][2], true);
	EXPECT_EQ(cpu.display[11][2], true);
	EXPECT_EQ(cpu.display[12][2], false);
	EXPECT_EQ(cpu.display[13][2], false);
	EXPECT_EQ(cpu.display[14][2], false);
	EXPECT_EQ(cpu.display[15][2], false);
	
	EXPECT_EQ(cpu.display[8][3], true);
	EXPECT_EQ(cpu.display[9][3], false);
	EXPECT_EQ(cpu.display[10][3], false);
	EXPECT_EQ(cpu.display[11][3], false);
	EXPECT_EQ(cpu.display[12][3], false);
	EXPECT_EQ(cpu.display[13][3], false);
	EXPECT_EQ(cpu.display[14][3], false);
	EXPECT_EQ(cpu.display[15][3], false);
	
	EXPECT_EQ(cpu.display[8][4], true);
	EXPECT_EQ(cpu.display[9][4], false);
	EXPECT_EQ(cpu.display[10][4], false);
	EXPECT_EQ(cpu.display[11][4], false);
	EXPECT_EQ(cpu.display[12][4], false);
	EXPECT_EQ(cpu.display[13][4], false);
	EXPECT_EQ(cpu.display[14][4], false);
	EXPECT_EQ(cpu.display[15][4], false);
	
	//draw '8' on screen coordinate (0, 0)
	//collision
	cpu.i = 0x78;
	runInstruction(cpu, 0xD015);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	EXPECT_EQ(cpu.display[0][0], false);
	EXPECT_EQ(cpu.display[1][0], false);
	EXPECT_EQ(cpu.display[2][0], false);
	EXPECT_EQ(cpu.display[3][0], false);
	EXPECT_EQ(cpu.display[4][0], false);
	EXPECT_EQ(cpu.display[5][0], false);
	EXPECT_EQ(cpu.display[6][0], false);
	EXPECT_EQ(cpu.display[7][0], false);
	
	EXPECT_EQ(cpu.display[0][1], false);
	EXPECT_EQ(cpu.display[1][1], false);
	EXPECT_EQ(cpu.display[2][1], false);
	EXPECT_EQ(cpu.display[3][1], false);
	EXPECT_EQ(cpu.display[4][1], false);
	EXPECT_EQ(cpu.display[5][1], false);
	EXPECT_EQ(cpu.display[6][1], false);
	EXPECT_EQ(cpu.display[7][1], false);
	
	EXPECT_EQ(cpu.display[0][2], false);
	EXPECT_EQ(cpu.display[1][2], true);
	EXPECT_EQ(cpu.display[2][2], true);
	EXPECT_EQ(cpu.display[3][2], false);
	EXPECT_EQ(cpu.display[4][2], false);
	EXPECT_EQ(cpu.display[5][2], false);
	EXPECT_EQ(cpu.display[6][2], false);
	EXPECT_EQ(cpu.display[7][2], false);
	
	EXPECT_EQ(cpu.display[0][3], false);
	EXPECT_EQ(cpu.display[1][3], false);
	EXPECT_EQ(cpu.display[2][3], false);
	EXPECT_EQ(cpu.display[3][3], false);
	EXPECT_EQ(cpu.display[4][3], false);
	EXPECT_EQ(cpu.display[5][3], false);
	EXPECT_EQ(cpu.display[6][3], false);
	EXPECT_EQ(cpu.display[7][3], false);
	
	EXPECT_EQ(cpu.display[0][4], false);
	EXPECT_EQ(cpu.display[1][4], false);
	EXPECT_EQ(cpu.display[2][4], false);
	EXPECT_EQ(cpu.display[3][4], false);
	EXPECT_EQ(cpu.display[4][4], false);
	EXPECT_EQ(cpu.display[5][4], false);
	EXPECT_EQ(cpu.display[6][4], false);
	EXPECT_EQ(cpu.display[7][4], false);

	//draw 'A' on screen coordinate (62, 15)
	//1 side warp x-axis
	cpu.i = 0x82;
	runInstruction(cpu, 0xD455);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	EXPECT_EQ(cpu.display[62][15], true);
	EXPECT_EQ(cpu.display[63][15], true);
	EXPECT_EQ(cpu.display[0][15], true);
	EXPECT_EQ(cpu.display[1][15], true);
	EXPECT_EQ(cpu.display[2][15], false);
	EXPECT_EQ(cpu.display[3][15], false);
	EXPECT_EQ(cpu.display[4][15], false);
	EXPECT_EQ(cpu.display[5][15], false);
	
	EXPECT_EQ(cpu.display[62][16], true);
	EXPECT_EQ(cpu.display[63][16], false);
	EXPECT_EQ(cpu.display[0][16], false);
	EXPECT_EQ(cpu.display[1][16], true);
	EXPECT_EQ(cpu.display[2][16], false);
	EXPECT_EQ(cpu.display[3][16], false);
	EXPECT_EQ(cpu.display[4][16], false);
	EXPECT_EQ(cpu.display[5][16], false);
	
	EXPECT_EQ(cpu.display[62][17], true);
	EXPECT_EQ(cpu.display[63][17], true);
	EXPECT_EQ(cpu.display[0][17], true);
	EXPECT_EQ(cpu.display[1][17], true);
	EXPECT_EQ(cpu.display[2][17], false);
	EXPECT_EQ(cpu.display[3][17], false);
	EXPECT_EQ(cpu.display[4][17], false);
	EXPECT_EQ(cpu.display[5][17], false);
	
	EXPECT_EQ(cpu.display[62][18], true);
	EXPECT_EQ(cpu.display[63][18], false);
	EXPECT_EQ(cpu.display[0][18], false);
	EXPECT_EQ(cpu.display[1][18], true);
	EXPECT_EQ(cpu.display[2][18], false);
	EXPECT_EQ(cpu.display[3][18], false);
	EXPECT_EQ(cpu.display[4][18], false);
	EXPECT_EQ(cpu.display[5][18], false);
	
	EXPECT_EQ(cpu.display[62][19], true);
	EXPECT_EQ(cpu.display[63][19], false);
	EXPECT_EQ(cpu.display[0][19], false);
	EXPECT_EQ(cpu.display[1][19], true);
	EXPECT_EQ(cpu.display[2][19], false);
	EXPECT_EQ(cpu.display[3][19], false);
	EXPECT_EQ(cpu.display[4][19], false);
	EXPECT_EQ(cpu.display[5][19], false);

	
	//draw 'B' on screen coordinate (31, 30)
	//1 side warp y-axis
	cpu.i = 0x87;
	runInstruction(cpu, 0xD675);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	EXPECT_EQ(cpu.display[31][30], true);
	EXPECT_EQ(cpu.display[32][30], true);
	EXPECT_EQ(cpu.display[33][30], true);
	EXPECT_EQ(cpu.display[34][30], false);
	EXPECT_EQ(cpu.display[35][30], false);
	EXPECT_EQ(cpu.display[36][30], false);
	EXPECT_EQ(cpu.display[37][30], false);
	EXPECT_EQ(cpu.display[38][30], false);
	
	EXPECT_EQ(cpu.display[31][31], true);
	EXPECT_EQ(cpu.display[32][31], false);
	EXPECT_EQ(cpu.display[33][31], false);
	EXPECT_EQ(cpu.display[34][31], true);
	EXPECT_EQ(cpu.display[35][31], false);
	EXPECT_EQ(cpu.display[36][31], false);
	EXPECT_EQ(cpu.display[37][31], false);
	EXPECT_EQ(cpu.display[38][31], false);
	
	EXPECT_EQ(cpu.display[31][0], true);
	EXPECT_EQ(cpu.display[32][0], true);
	EXPECT_EQ(cpu.display[33][0], true);
	EXPECT_EQ(cpu.display[34][0], false);
	EXPECT_EQ(cpu.display[35][0], false);
	EXPECT_EQ(cpu.display[36][0], false);
	EXPECT_EQ(cpu.display[37][0], false);
	EXPECT_EQ(cpu.display[38][0], false);
	
	EXPECT_EQ(cpu.display[31][1], true);
	EXPECT_EQ(cpu.display[32][1], false);
	EXPECT_EQ(cpu.display[33][1], false);
	EXPECT_EQ(cpu.display[34][1], true);
	EXPECT_EQ(cpu.display[35][1], false);
	EXPECT_EQ(cpu.display[36][1], false);
	EXPECT_EQ(cpu.display[37][1], false);
	EXPECT_EQ(cpu.display[38][1], false);
	
	EXPECT_EQ(cpu.display[31][2], true);
	EXPECT_EQ(cpu.display[32][2], true);
	EXPECT_EQ(cpu.display[33][2], true);
	EXPECT_EQ(cpu.display[34][2], false);
	EXPECT_EQ(cpu.display[35][2], false);
	EXPECT_EQ(cpu.display[36][2], false);
	EXPECT_EQ(cpu.display[37][2], false);
	EXPECT_EQ(cpu.display[38][2], false);
	
	//2 side warp
	cpu.cls();	//clear the screen
	
		//load vertical sprite and horizontal sprite (8 bits wide) in memory
	
	cpu.memory [0x900] = 0xFF;	//horizontal line sprite
	
	cpu.memory [0x901] = 0x80;	//vertical line sprite
	cpu.memory [0x902] = 0x80;
	cpu.memory [0x903] = 0x80;
	cpu.memory [0x904] = 0x80;
	cpu.memory [0x905] = 0x80;
	cpu.memory [0x906] = 0x80;
	cpu.memory [0x907] = 0x80;
	cpu.memory [0x908] = 0x80;
	
	//(60,28)
			
	cpu.i = 0x900;
	runInstruction(cpu, 0xD891);
	EXPECT_EQ(cpu.v[0xF], 0);
	
	EXPECT_EQ(cpu.display[60][28], true);
	EXPECT_EQ(cpu.display[61][28], true);
	EXPECT_EQ(cpu.display[62][28], true);
	EXPECT_EQ(cpu.display[63][28], true);
	EXPECT_EQ(cpu.display[0][28], true);
	EXPECT_EQ(cpu.display[1][28], true);
	EXPECT_EQ(cpu.display[2][28], true);
	EXPECT_EQ(cpu.display[3][28], true);
	
	cpu.i = 0x901;
	runInstruction(cpu, 0xD898);
	EXPECT_EQ(cpu.v[0xF], 1);
	
	EXPECT_EQ(cpu.display[60][28], false);
	EXPECT_EQ(cpu.display[61][28], true);
	EXPECT_EQ(cpu.display[62][28], true);
	EXPECT_EQ(cpu.display[63][28], true);
	EXPECT_EQ(cpu.display[0][28], true);
	EXPECT_EQ(cpu.display[1][28], true);
	EXPECT_EQ(cpu.display[2][28], true);
	EXPECT_EQ(cpu.display[3][28], true);
	
	EXPECT_EQ(cpu.display[60][29], true);
	EXPECT_EQ(cpu.display[61][29], false);
	EXPECT_EQ(cpu.display[62][29], false);
	EXPECT_EQ(cpu.display[63][29], false);
	EXPECT_EQ(cpu.display[0][29], false);
	EXPECT_EQ(cpu.display[1][29], false);
	EXPECT_EQ(cpu.display[2][29], false);
	EXPECT_EQ(cpu.display[3][29], false);
	
	EXPECT_EQ(cpu.display[60][30], true);
	EXPECT_EQ(cpu.display[61][30], false);
	EXPECT_EQ(cpu.display[62][30], false);
	EXPECT_EQ(cpu.display[63][30], false);
	EXPECT_EQ(cpu.display[0][30], false);
	EXPECT_EQ(cpu.display[1][30], false);
	EXPECT_EQ(cpu.display[2][30], false);
	EXPECT_EQ(cpu.display[3][30], false);
	
	EXPECT_EQ(cpu.display[60][31], true);
	EXPECT_EQ(cpu.display[61][31], false);
	EXPECT_EQ(cpu.display[62][31], false);
	EXPECT_EQ(cpu.display[63][31], false);
	EXPECT_EQ(cpu.display[0][31], false);
	EXPECT_EQ(cpu.display[1][31], false);
	EXPECT_EQ(cpu.display[2][31], false);
	EXPECT_EQ(cpu.display[3][31], false);
	
	EXPECT_EQ(cpu.display[60][0], true);
	EXPECT_EQ(cpu.display[61][0], false);
	EXPECT_EQ(cpu.display[62][0], false);
	EXPECT_EQ(cpu.display[63][0], false);
	EXPECT_EQ(cpu.display[0][0], false);
	EXPECT_EQ(cpu.display[1][0], false);
	EXPECT_EQ(cpu.display[2][0], false);
	EXPECT_EQ(cpu.display[3][0], false);
	
	EXPECT_EQ(cpu.display[60][1], true);
	EXPECT_EQ(cpu.display[61][1], false);
	EXPECT_EQ(cpu.display[62][1], false);
	EXPECT_EQ(cpu.display[63][1], false);
	EXPECT_EQ(cpu.display[0][1], false);
	EXPECT_EQ(cpu.display[1][1], false);
	EXPECT_EQ(cpu.display[2][1], false);
	EXPECT_EQ(cpu.display[3][1], false);
	
	EXPECT_EQ(cpu.display[60][2], true);
	EXPECT_EQ(cpu.display[61][2], false);
	EXPECT_EQ(cpu.display[62][2], false);
	EXPECT_EQ(cpu.display[63][2], false);
	EXPECT_EQ(cpu.display[0][2], false);
	EXPECT_EQ(cpu.display[1][2], false);
	EXPECT_EQ(cpu.display[2][2], false);
	EXPECT_EQ(cpu.display[3][2], false);
	
	EXPECT_EQ(cpu.display[60][3], true);
	EXPECT_EQ(cpu.display[61][3], false);
	EXPECT_EQ(cpu.display[62][3], false);
	EXPECT_EQ(cpu.display[63][3], false);
	EXPECT_EQ(cpu.display[0][3], false);
	EXPECT_EQ(cpu.display[1][3], false);
	EXPECT_EQ(cpu.display[2][3], false);
	EXPECT_EQ(cpu.display[3][3], false);
	 
	
	
}

TEST_F(Chip8Test, SKP_Vx) {
/*
Ex9E - SKP Vx
Skip next instruction if key with the value of Vx is pressed.

Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
*/


	cpu.v[0x5] = 0xb;
	cpu.setKeypad(0xf, true);
	cpu.v[0x7] = 0xf;

	//button 0 is not pressed 
	//v0 is set to 0
	
	runInstruction(cpu, 0xE09E);
	EXPECT_EQ(cpu.pc, 0x202);
	
	//button F is pressed
	//v5 is set to B
	
	runInstruction(cpu, 0xE59E);
	EXPECT_EQ(cpu.pc, 0x204);
	
	//button F is pressed
	//v7 is set to F
	
	runInstruction(cpu, 0xE79E);
	EXPECT_EQ(cpu.pc, 0x208);

}

TEST_F(Chip8Test, SKNP_Vx) {
/*
ExA1 - SKNP Vx
Skip next instruction if key with the value of Vx is not pressed.

Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
*/

	cpu.v[0x5] = 0xb;
	cpu.setKeypad(0xf, true);
	cpu.v[0x7] = 0xf;

	//button 0 is not pressed 
	//v0 is set to 0
	
	runInstruction(cpu, 0xE0A1);
	EXPECT_EQ(cpu.pc, 0x204);
	
	//button F is pressed
	//v5 is set to B
	
	runInstruction(cpu, 0xE5A1);
	EXPECT_EQ(cpu.pc, 0x208);
	
	//button F is pressed
	//v7 is set to F
	
	runInstruction(cpu, 0xE7A1);
	EXPECT_EQ(cpu.pc, 0x20A);

}


TEST_F(Chip8Test, setKeypad) {

	// test keypad input
	
	EXPECT_EQ(cpu.keypad[0x0], false);
	cpu.setKeypad(0x0, true);
	EXPECT_EQ(cpu.keypad[0x0], true);
	cpu.setKeypad(0x0, false);
	EXPECT_EQ(cpu.keypad[0x0], false);
	
	EXPECT_EQ(cpu.keypad[0xF], false);
	cpu.setKeypad(0xF, true);
	EXPECT_EQ(cpu.keypad[0xF], true);
	cpu.setKeypad(0xF, false);
	EXPECT_EQ(cpu.keypad[0xF], false);
	
	EXPECT_THROW(cpu.setKeypad(0x10, true), std::runtime_error);
	
}

TEST_F(Chip8Test, LD_Vx_K) {
/*
Fx0A - LD Vx, K
Wait for a key press, store the value of the key in Vx.

All execution stops until a key is pressed, then the value of that key is stored in Vx.
*/

	//load instructions in memory

	cpu.memory[0x200] = 0x00;	//CLS
	cpu.memory[0x201] = 0xE0;

	cpu.memory[0x202] = 0x00;	//CLS
	cpu.memory[0x203] = 0xE0;
	
	cpu.memory[0x204] = 0x00;	//CLS
	cpu.memory[0x205] = 0xE0;
	
	cpu.memory[0x206] = 0xF0;	//LD_Vx_K
	cpu.memory[0x207] = 0x0A;
	
	cpu.memory[0x208] = 0x00;	//CLS
	cpu.memory[0x209] = 0xE0;
	
	cpu.memory[0x20A] = 0xF0;	//LD_Vx_K
	cpu.memory[0x20B] = 0x0A;
	
	cpu.memory[0x20C] = 0x00;	//CLS
	cpu.memory[0x20D] = 0xE0;
	
	cpu.memory[0x20E] = 0xF5;	//LD_Vx_K
	cpu.memory[0x20F] = 0x0A;
	
	cpu.cycle();	//CLS
	cpu.cycle();	//CLS
	cpu.cycle();	//CLS
	
	
	//no button is pressed -->  //don't excute --> wait on address 0x206
	cpu.cycle();	//LD_Vx_K
	EXPECT_EQ(cpu.pc, 0x206);
	cpu.cycle();	//LD_Vx_K
	EXPECT_EQ(cpu.pc, 0x206);
	cpu.cycle();	//LD_Vx_K
	EXPECT_EQ(cpu.pc, 0x206);
	
	//button is pressed, excute opcode, increase pc
	cpu.setKeypad(0x5, true);
	cpu.cycle();	//LD_Vx_K
	EXPECT_EQ(cpu.v[0x0], 0x5);
	EXPECT_EQ(cpu.pc, 0x208);
	
	cpu.setKeypad(0x5, false);
	cpu.setKeypad(0x8, true);
	cpu.cycle();	//CLS
	
	//button is pressed, excute opcode, increase pc
	cpu.cycle();	//LD_Vx_K
	EXPECT_EQ(cpu.v[0x0], 0x8);
	EXPECT_EQ(cpu.pc, 0x20C);
	
	cpu.cycle();	//CLS
	cpu.setKeypad(0x8, false);
	
	
	cpu.setKeypad(0x3, true);
	cpu.setKeypad(0xB, true);
	
	//2 buttons are pressed, excute opcode, increase pc --> only highest button will be detected
	
	cpu.cycle();	//LD_Vx_K
	EXPECT_EQ(cpu.v[0x5], 0xB);
	EXPECT_EQ(cpu.pc, 0x210);
	

}

TEST_F(Chip8Test, LD_DT_Vx) {
/*
Fx15 - LD DT, Vx
Set delay timer = Vx.

DT is set equal to the value of Vx.
*/
	std::chrono::milliseconds milsec(1);


	cpu.memory[0x200] = 0xF0;	//LD_DT_Vx
	cpu.memory[0x201] = 0x15;

	cpu.memory[0x202] = 0x00;	//CLS
	cpu.memory[0x203] = 0xE0;

	cpu.memory[0x202] = 0x00;	//CLS
	cpu.memory[0x203] = 0xE0;

	cpu.v[0x0] = 0xFF;
	
	EXPECT_EQ(cpu.delayRegister, 0x00);
	cpu.cycle();
	EXPECT_EQ(cpu.delayRegister, 0xFF);
	
	std::this_thread::sleep_for(milsec * 2000);
	
	cpu.cycle();
	EXPECT_NE(cpu.delayRegister, 0x00);
	EXPECT_LT(cpu.delayRegister, 0xFF);
	
	std::this_thread::sleep_for(milsec * 2260);
	
	cpu.cycle();
	EXPECT_EQ(cpu.delayRegister, 0x00);

}

TEST_F(Chip8Test, LD_Vx_DT) {
/*
Fx07 - LD Vx, DT
Set Vx = delay timer value.

The value of DT is placed into Vx.
*/
	
	std::chrono::milliseconds milsec(1);

	cpu.memory[0x200] = 0xF0;	//LD_DT_Vx
	cpu.memory[0x201] = 0x15;

	cpu.memory[0x202] = 0xFA;	//LD_Vx_DT
	cpu.memory[0x203] = 0x07;

	cpu.memory[0x204] = 0xFA;	//LD_Vx_DT
	cpu.memory[0x205] = 0x07;



	
	cpu.v[0x0] = 0xFF;
	
	cpu.cycle();	//LD_DT_Vx
	
	std::this_thread::sleep_for(milsec * 16.667);
	
	cpu.cycle();	//LD_Vx_DT
	
	EXPECT_LT(cpu.v[0xA], 0xFF);
	EXPECT_NE(cpu.v[0xA], 0x0);
	
	std::this_thread::sleep_for(milsec * 4233.333);
	
	cpu.cycle();	//LD_Vx_DT
	EXPECT_EQ(cpu.v[0xA], 0x0);
	
	
}

TEST_F(Chip8Test, LD_ST_Vx) {
/*
Fx18 - LD ST, Vx
Set sound timer = Vx.

ST is set equal to the value of Vx.
*/

	std::chrono::milliseconds milsec(1);


	cpu.memory[0x200] = 0xF0;	//LD_ST_Vx
	cpu.memory[0x201] = 0x18;

	cpu.memory[0x202] = 0x00;	//CLS
	cpu.memory[0x203] = 0xE0;

	cpu.memory[0x204] = 0xF0;	//LD_ST_Vx
	cpu.memory[0x205] = 0x18;

	cpu.v[0x0] = 0xFF;
	
	EXPECT_EQ(cpu.soundRegister, 0x00);
	cpu.cycle();	//LD_ST_Vx
	EXPECT_EQ(cpu.soundRegister, 0xFF);
	
	std::this_thread::sleep_for(milsec * 16.667);
	
	EXPECT_EQ(cpu.getSoundSignal(), true);
	EXPECT_LT(cpu.soundRegister, 0xFF);
	
	std::this_thread::sleep_for(milsec * 4233.333);
	
	cpu.cycle();	//CLS
	EXPECT_EQ(cpu.soundRegister, 0x0);
	EXPECT_EQ(cpu.getSoundSignal(), false);
	
	cpu.cycle();	//LD_ST_Vx
	EXPECT_EQ(cpu.getSoundSignal(), true);
	std::this_thread::sleep_for(milsec * 4260);
	EXPECT_EQ(cpu.getSoundSignal(), false);

}

TEST_F(Chip8Test, ADD_I_Vx) {
/*
Fx1E - ADD I, Vx

Set I = I + Vx.
The values of I and Vx are added, and the results are stored in I.
*/

	cpu.v[0x6] = 0x0;
	cpu.v[0x7] = 0xFF;
	cpu.v[0x8] = 0x2;
	
	runInstruction(cpu, 0xF61E);
	EXPECT_EQ(cpu.i, 0x0000);
	
	runInstruction(cpu, 0xF71E);
	EXPECT_EQ(cpu.i, 0x00FF);
	
	runInstruction(cpu, 0xF71E);
	EXPECT_EQ(cpu.i, 0x01FE);
	
	cpu.i = 0x0FFD;
	runInstruction(cpu, 0xF81E);
	EXPECT_EQ(cpu.i, 0x0FFF);
	
	EXPECT_THROW(runInstruction(cpu, 0xF81E), std::runtime_error);
	
	

}

TEST_F(Chip8Test, LD_F_Vx) {
/*
Fx29 - LD F, Vx

Set I = location of sprite for digit Vx.
The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
*/

	cpu.v[0x0] = 0x0;
	cpu.v[0x1] = 0x1;
	cpu.v[0x2] = 0x2;
	cpu.v[0x3] = 0x3;
	cpu.v[0x4] = 0x4;
	cpu.v[0x5] = 0x5;
	cpu.v[0x6] = 0x6;
	cpu.v[0x7] = 0x7;
	cpu.v[0x8] = 0x8;
	cpu.v[0x9] = 0x9;
	cpu.v[0xA] = 0xA;
	cpu.v[0xB] = 0xB;
	cpu.v[0xC] = 0xC;
	cpu.v[0xD] = 0xD;
	cpu.v[0xE] = 0xE;
	cpu.v[0xF] = 0xF; 

	runInstruction(cpu, 0xF029);
	EXPECT_EQ(cpu.i, 0x50);
	
	runInstruction(cpu, 0xF129);
	EXPECT_EQ(cpu.i, 0x55);
	
	runInstruction(cpu, 0xF229);
	EXPECT_EQ(cpu.i, 0x5A);
	
	runInstruction(cpu, 0xF329);
	EXPECT_EQ(cpu.i, 0x5F);
	
	runInstruction(cpu, 0xF429);
	EXPECT_EQ(cpu.i, 0x64);
	
	runInstruction(cpu, 0xF529);
	EXPECT_EQ(cpu.i, 0x69);
	
	runInstruction(cpu, 0xF629);
	EXPECT_EQ(cpu.i, 0x6E);
	
	runInstruction(cpu, 0xF729);
	EXPECT_EQ(cpu.i, 0x73);
	
	runInstruction(cpu, 0xF829);
	EXPECT_EQ(cpu.i, 0x78);
	
	runInstruction(cpu, 0xF929);
	EXPECT_EQ(cpu.i, 0x7D);
	
	runInstruction(cpu, 0xFA29);
	EXPECT_EQ(cpu.i, 0x82);
	
	runInstruction(cpu, 0xFB29);
	EXPECT_EQ(cpu.i, 0x87);
	
	runInstruction(cpu, 0xFC29);
	EXPECT_EQ(cpu.i, 0x8C);
	
	runInstruction(cpu, 0xFD29);
	EXPECT_EQ(cpu.i, 0x91);
	
	runInstruction(cpu, 0xFE29);
	EXPECT_EQ(cpu.i, 0x96);
	
	runInstruction(cpu, 0xFF29);
	EXPECT_EQ(cpu.i, 0x9B);
	
}

TEST_F(Chip8Test, LD_B_Vx) {
/*
Fx33 - LD B, Vx

Store BCD representation of Vx in memory locations I, I+1, and I+2.
The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
*/
	cpu.v[0x0] = 4;
	cpu.v[0x1] = 217;
	cpu.v[0x2] = 46;
	cpu.v[0x3] = 84;
	cpu.v[0x4] = 38;
	cpu.v[0x5] = 5;

	cpu.i = 0x500;

	runInstruction(cpu, 0xF033);
	EXPECT_EQ(cpu.memory[0x500], 0x0);
	EXPECT_EQ(cpu.memory[0x501], 0x0);
	EXPECT_EQ(cpu.memory[0x502], 0x4);
	
	runInstruction(cpu, 0xF133);
	EXPECT_EQ(cpu.memory[0x500], 0x2);
	EXPECT_EQ(cpu.memory[0x501], 0x1);
	EXPECT_EQ(cpu.memory[0x502], 0x7);
	
	runInstruction(cpu, 0xF233);
	EXPECT_EQ(cpu.memory[0x500], 0x0);
	EXPECT_EQ(cpu.memory[0x501], 0x4);
	EXPECT_EQ(cpu.memory[0x502], 0x6);
	
	runInstruction(cpu, 0xF333);
	EXPECT_EQ(cpu.memory[0x500], 0x0);
	EXPECT_EQ(cpu.memory[0x501], 0x8);
	EXPECT_EQ(cpu.memory[0x502], 0x4);
	
	runInstruction(cpu, 0xF433);
	EXPECT_EQ(cpu.memory[0x500], 0x0);
	EXPECT_EQ(cpu.memory[0x501], 0x3);
	EXPECT_EQ(cpu.memory[0x502], 0x8);
	
	runInstruction(cpu, 0xF533);
	EXPECT_EQ(cpu.memory[0x500], 0x0);
	EXPECT_EQ(cpu.memory[0x501], 0x0);
	EXPECT_EQ(cpu.memory[0x502], 0x5);

}

TEST_F(Chip8Test, LD_I_Vx) {
/*
Fx55 - LD [I], Vx

Store registers V0 through Vx in memory starting at location I.
The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
*/


	//random data
	cpu.v[0x0] = 4;
	cpu.v[0x1] = 217;
	cpu.v[0x2] = 46;
	cpu.v[0x3] = 84;
	cpu.v[0x4] = 38;
	cpu.v[0x5] = 5;
	cpu.v[0x6] = 178;
	cpu.v[0x7] = 176;
	cpu.v[0x8] = 109;
	cpu.v[0x9] = 80;
	cpu.v[0xA] = 141;
	cpu.v[0xB] = 238;
	cpu.v[0xC] = 203;
	cpu.v[0xD] = 25;
	cpu.v[0xE] = 164;
	cpu.v[0xF] = 150;
	
	cpu.i = 0x500;
	runInstruction(cpu, 0xF055);
	EXPECT_EQ(cpu.memory[0x500], 4);
	EXPECT_EQ(cpu.memory[0x501], 0);
	
	cpu.i = 0x600;
	runInstruction(cpu, 0xF155);
	EXPECT_EQ(cpu.memory[0x600], 4);
	EXPECT_EQ(cpu.memory[0x601], 217);
	EXPECT_EQ(cpu.memory[0x602], 0);
	
	cpu.i = 0x700;
	runInstruction(cpu, 0xFF55);
	EXPECT_EQ(cpu.memory[0x700], 4);
	EXPECT_EQ(cpu.memory[0x701], 217);
	EXPECT_EQ(cpu.memory[0x702], 46);
	EXPECT_EQ(cpu.memory[0x703], 84);
	EXPECT_EQ(cpu.memory[0x704], 38);
	EXPECT_EQ(cpu.memory[0x705], 5);
	EXPECT_EQ(cpu.memory[0x706], 178);
	EXPECT_EQ(cpu.memory[0x707], 176);
	EXPECT_EQ(cpu.memory[0x708], 109);
	EXPECT_EQ(cpu.memory[0x709], 80);
	EXPECT_EQ(cpu.memory[0x70A], 141);
	EXPECT_EQ(cpu.memory[0x70B], 238);
	EXPECT_EQ(cpu.memory[0x70C], 203);
	EXPECT_EQ(cpu.memory[0x70D], 25);
	EXPECT_EQ(cpu.memory[0x70E], 164);
	EXPECT_EQ(cpu.memory[0x70F], 150);
	EXPECT_EQ(cpu.memory[0x710], 0);
	
	cpu.i = 0xFFD;
	runInstruction(cpu, 0xF255);
	EXPECT_EQ(cpu.memory[0xFFD], 4);
	EXPECT_EQ(cpu.memory[0xFFE], 217);
	EXPECT_EQ(cpu.memory[0xFFF], 46);
	
	EXPECT_THROW(runInstruction(cpu, 0xF355), std::runtime_error);
	
}

TEST_F(Chip8Test, LD_Vx_I) {
/*
Fx65 - LD Vx, [I]

Read registers V0 through Vx from memory starting at location I.
The interpreter reads values from memory starting at location I into registers V0 through Vx.
*/

	cpu.i = 0x50;
	runInstruction(cpu, 0xF065);
	
	EXPECT_EQ(cpu.v[0x0], 0xF0);
	EXPECT_EQ(cpu.v[0x1], 0x0);
	
	cpu.i = 0x55;
	runInstruction(cpu, 0xF465);
	
	EXPECT_EQ(cpu.v[0x0], 0x20);
	EXPECT_EQ(cpu.v[0x1], 0x60);
	EXPECT_EQ(cpu.v[0x2], 0x20);
	EXPECT_EQ(cpu.v[0x3], 0x20);
	EXPECT_EQ(cpu.v[0x4], 0x70);
	EXPECT_EQ(cpu.v[0x5], 0x0);
	
	cpu.i = 0x5A;
	runInstruction(cpu, 0xFF65);
	
	EXPECT_EQ(cpu.v[0x0], 0xF0);
	EXPECT_EQ(cpu.v[0x1], 0x10);
	EXPECT_EQ(cpu.v[0x2], 0xF0);
	EXPECT_EQ(cpu.v[0x3], 0x80);
	EXPECT_EQ(cpu.v[0x4], 0xF0);
	EXPECT_EQ(cpu.v[0x5], 0xF0);
	EXPECT_EQ(cpu.v[0x6], 0x10);
	EXPECT_EQ(cpu.v[0x7], 0xF0);
	EXPECT_EQ(cpu.v[0x8], 0x10);
	EXPECT_EQ(cpu.v[0x9], 0xF0);
	EXPECT_EQ(cpu.v[0xA], 0x90);
	EXPECT_EQ(cpu.v[0xB], 0x90);
	EXPECT_EQ(cpu.v[0xC], 0xF0);
	EXPECT_EQ(cpu.v[0xD], 0x10);
	EXPECT_EQ(cpu.v[0xE], 0x10);
	EXPECT_EQ(cpu.v[0xF], 0xF0);

	
}
