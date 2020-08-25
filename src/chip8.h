#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
//#include <iterator>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <random>
#include <chrono>

class Chip8 {
	public:

	
		Chip8() : generator(std::chrono::system_clock::now().time_since_epoch().count()) {}
		/*Chip8(std::string romName) : generator(std::chrono::system_clock::now().time_since_epoch().count()) {
			this->init();
			this->loadRom(std::string romName); 
			}*/
		
		void init();
		void loadRom(std::string romName);
		void cycle();
		void setKeypad(unsigned int key, bool state);
		const bool& getSoundSignal() {
			updateTimers();
			return sound;
			}
		void resetUpdateScreen() {
			updateScreen = false;
		}
		
		const bool& getUpdateScreen() {
			return updateScreen;
		}
		
		bool getPixel(unsigned int x,unsigned int y) {return display[x][y];}
		 
	
	private:

		///GTEST///
		friend class Chip8Test_init_Test;
		friend class Chip8Test_getOpcode_Test;
		friend class Chip8Test_loadRom_Test;
		friend class Chip8Test_CLS_Test;
		friend class Chip8Test_RET_Test;
		friend class Chip8Test_JPaddr_Test;
		friend class Chip8Test_CALLaddr_Test;
		friend class Chip8Test_SE_Vx_byte_Test;
		friend class Chip8Test_SNE_Vx_byte_Test;
		friend class Chip8Test_SE_Vx_Vy_Test;
		friend class Chip8Test_LD_Vx_byte_Test;
		friend class Chip8Test_ADD_Vx_byte_Test;
		friend class Chip8Test_LD_Vx_Vy_Test;
		friend class Chip8Test_OR_Vx_Vy_Test;
		friend class Chip8Test_AND_Vx_Vy_Test;
		friend class Chip8Test_XOR_Vx_Vy_Test;
		friend class Chip8Test_ADD_Vx_Vy_Test;
		friend class Chip8Test_SUB_Vx_Vy_Test;
		friend class Chip8Test_SHR_Vx_Vy_Test;
		friend class Chip8Test_SUBN_Vx_Vy_Test;
		friend class Chip8Test_SHL_Vx_Vy_Test;
		friend class Chip8Test_SNE_Vx_Vy_Test;
		friend class Chip8Test_LD_I_addr_Test;
		friend class Chip8Test_JP_V0_addr_Test;
		friend class Chip8Test_RND_Vx_byte_Test;
		friend class Chip8Test_DRW_Vx_Vy_nibble_Test;
		friend class Chip8Test_SKP_Vx_Test;
		friend class Chip8Test_SKNP_Vx_Test;
		friend class Chip8Test_LD_Vx_DT_Test;
		friend class Chip8Test_LD_Vx_K_Test;
		friend class Chip8Test_LD_DT_Vx_Test;
		friend class Chip8Test_LD_ST_Vx_Test;
		friend class Chip8Test_setKeypad_Test;
		friend class Chip8Test_ADD_I_Vx_Test;
		friend class Chip8Test_LD_F_Vx_Test;
		friend class Chip8Test_LD_B_Vx_Test;
		friend class Chip8Test_LD_I_Vx_Test;
		friend class Chip8Test_LD_Vx_I_Test;
		///GTEST///
		friend void runInstruction(Chip8&, const uint16_t&);
		
		//cpu components
			uint8_t memory[4096] = {0};
			uint16_t stack[16] = {0};
			
			//registers
			uint8_t v[16] = {0}; //general purpose 8-bit registers
			uint16_t i = 0; //generally used to store memory addresses
			uint8_t soundRegister = 0;
			uint8_t delayRegister = 0;
			
			//pseudo registers
			uint16_t pc = 0; //program counter: is used to store the currently executing address
			uint8_t sp = 0; //stack pointer: is used to point to the topmost level of the stack
			
		//hardware
			bool display[64][32]= {{0}};
			bool keypad[16] = {0};
			bool sound = false;
			
		//emulation
			const uint16_t Program_StartAddress = 0x200;
			const uint16_t Font_StartAddress = 0x50;
			uint16_t opcode = 0;
			bool usingStack = false;
			std::size_t romSize = 0;
			char* rom = nullptr;
			std::default_random_engine generator;
			unsigned int timerFrequency = 60;
			decltype(std::chrono::high_resolution_clock().now()) startDelay, startDelaySound;
			double remainderDecimals = 0.0, remainderDecimalsSound = 0.0;
			bool updateScreen = false;
			
			
			//member functions	
				void loadFontData();
				void setFunctionTable();
				void getOpcode();
				void exec();
				void pcInc();
				void readBinaryData(std::string fileName);
				void updateTimers();
			
		//opcode
			void cls();			// 00E0 | Clear the display
			void ret();			// 00EE | Return from a subroutine.
			void JPaddr();			// 1nnn | JP addr Jump to location nnn.
			void CALLaddr(); 		// 2nnn | Call subroutine at nnn.
			void SE_Vx_byte();		// 3xkk | SE Vx, byte
			void SNE_Vx_byte();		// 4xkk | SNE Vx, byte
			void SE_Vx_Vy();		// 5xy0 | Skip next instruction if Vx = Vy.
			void LD_Vx_byte();		// 6xkk | Set Vx = kk.
			void ADD_Vx_byte();		// 7xkk | Set Vx = Vx + kk.
			void LD_Vx_Vy();		// 8xy0 | Set Vx = Vy.
			void OR_Vx_Vy();		// 8xy1 | Set Vx = Vx OR Vy.
			void AND_Vx_Vy();		// 8xy2 | Set Vx = Vx AND Vy.
			void XOR_Vx_Vy();		// 8xy3 | Set Vx = Vx XOR Vy.
			void ADD_Vx_Vy();		// 8xy4 | Set Vx = Vx + Vy, set VF = carry.
			void SUB_Vx_Vy();		// 8xy5 | Set Vx = Vx - Vy, set VF = NOT borrow.
			void SHR_Vx_Vy();		// 8xy6 | Set Vx = Vx SHR 1.
			void SUBN_Vx_Vy();		// 8xy7 | Set Vx = Vy - Vx, set VF = NOT borrow.
			void SHL_Vx_Vy();		// 8xyE | Set Vx = Vx SHL 1.
			void SNE_Vx_Vy();		// 9xy0 | Skip next instruction if Vx != Vy.
			void LD_I_addr();		// Annn | Set I = nnn.
			void JP_V0_addr();		// Bnnn | Jump to location nnn + V0.
			void RND_Vx_byte();		// Cxkk | Set Vx = random byte AND kk.
			void DRW_Vx_Vy_nibble();	// Dxyn | Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
			void SKP_Vx();			// Ex9E | Skip next instruction if key with the value of Vx is pressed.
			void SKNP_Vx();			// ExA1 | Skip next instruction if key with the value of Vx is not pressed.
			void LD_Vx_DT();		// Fx07 | Set Vx = delay timer value.
			void LD_Vx_K();			// Fx0A | Wait for a key press, store the value of the key in Vx.
			void LD_DT_Vx();		// Fx15 | Set delay timer = Vx.
			void LD_ST_Vx();		// Fx18 | Set sound timer = Vx.
			void ADD_I_Vx();		// Fx1E | ADD I, Vx
			void LD_F_Vx();			// Fx29 | LD F, Vx
			void LD_B_Vx();			// Fx33 | LD B, Vx
			void LD_I_Vx();			// Fx55 | LD [I], Vx
			void LD_Vx_I();			// Fx65 | LD Vx, [I]
			
			void unknownOpcode();

		//function array
			using function = void (Chip8::*)();
			
			function opTable0[16]{&Chip8::unknownOpcode};
			void opTable0_Func();
			
			function opTable8[16]{&Chip8::unknownOpcode};
			void opTable8_Func();
			
			function opTableE[256]{&Chip8::unknownOpcode};
			void opTableE_Func();
			
			function opTableF[256]{&Chip8::unknownOpcode};
			void opTableF_Func();
			
			function opTable[16]{&Chip8::unknownOpcode};

};

void Chip8::updateTimers() {
	
	if (delayRegister != 0) {
		
		auto currentTick = std::chrono::high_resolution_clock().now();
		double dt = std::chrono::duration<double, std::chrono::milliseconds::period>(currentTick - startDelay).count();
		
		double maxMilSeconds = ((1/static_cast<double>(timerFrequency)) * 1000);
		unsigned int reduceTimes = static_cast<double>((dt / maxMilSeconds) + remainderDecimals);
		
		
		for(unsigned it = 0; it != reduceTimes; ++it) {
		
			delayRegister -= (delayRegister > 0) ? 1 : 0;
		
		}
	
		remainderDecimals = (dt / maxMilSeconds) - static_cast<int>((dt / maxMilSeconds));
		startDelay = std::chrono::high_resolution_clock().now();

	
	}
	
	if (soundRegister != 0) {
		
		auto currentTick = std::chrono::high_resolution_clock().now();
		double dt = std::chrono::duration<double, std::chrono::milliseconds::period>(currentTick - startDelaySound).count();
		
		double maxMilSeconds = ((1/static_cast<double>(timerFrequency)) * 1000);
		unsigned int reduceTimes = static_cast<double>((dt / maxMilSeconds) + remainderDecimalsSound);
		
		
		for(unsigned it = 0; it != reduceTimes; ++it) {
		
			soundRegister -= (soundRegister > 0) ? 1 : 0;
		
		}
	
		remainderDecimalsSound = (dt / maxMilSeconds) - static_cast<int>((dt / maxMilSeconds));
		startDelaySound = std::chrono::high_resolution_clock().now();

		sound = soundRegister != 0 ? true : false;
	
	}

}

void Chip8::setKeypad(unsigned int key, bool state) {
	
	if (key > 0xF)
		throw std::runtime_error("Only keypad 0 to F is allowed!");
	keypad[key] = state;
	
}

void Chip8::loadRom(std::string fileName) {

	const unsigned int memSize = 4096;

	readBinaryData(fileName);

	
	if (romSize >= memSize) throw std::runtime_error("Rom is too big for the Chip8 memory!");
	
	//load rom to memory
	for(unsigned it = 0; it != romSize; ++it) {
	
		memory[it + Program_StartAddress] = rom[it];
	
	}
	
	
	delete[] rom;
	rom = nullptr;
	
	
}

void Chip8::readBinaryData(std::string fileName) {
	
	std::ifstream binary(fileName, std::ios::in|std::ios::binary|std::ios::ate);
	std::streampos size;

	if(binary.is_open()) {

	
	size = binary.tellg();
	binary.seekg(0, std::ios::beg);
	rom = new char[size];
	binary.read(rom, size);
	binary.close();
	}
	else throw std::runtime_error("Couldn't open rom file!");
	
	romSize = size;
}

void Chip8::init() {
	//load font data to memory 0x000 to 0x1FF
	loadFontData();
	
	// set the program counter to the beginning of the program
	pc = Program_StartAddress;
	
	// initialize the function pointer array for the opcode
	setFunctionTable();
	
	
	

}

void Chip8::loadFontData() {
	//The font data should be stored in the interpreter area of Chip-8 memory (0x000 to 0x1FF)
	
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
			      
	for(unsigned int FontDataIterator = 0; FontDataIterator != std::end(fontData) - std::begin(fontData) ; ++FontDataIterator) {
		
		memory[Font_StartAddress + FontDataIterator] = fontData[FontDataIterator];
	
	}

}

void Chip8::getOpcode() {
	opcode = (memory[pc] << 8) | (memory[pc + 1]);

}

void Chip8::exec() {
	(this->*opTable[(opcode & 0xF000) >> 12])();

}

void Chip8::cycle() {
	//updateTimers();
	if (delayRegister != 0)
		--delayRegister;
	
	if (soundRegister != 0) {
		--soundRegister;
		sound = soundRegister != 0 ? true : false;
	}
		
	getOpcode();
	pcInc();
	exec();

}

void Chip8::pcInc() { 
	pc += 2;
	if (pc > 0xFFF) {throw std::runtime_error("The PC points outside the memory space!");}
		
	
}

void Chip8::opTable0_Func() {
	(this->*opTable0[opcode & 0x000F])();


}

void Chip8::opTable8_Func() {
	(this->*opTable8[opcode & 0x000F])();


}

void Chip8::opTableE_Func() {
	(this->*opTableE[opcode & 0x00FF])();


}

void Chip8::opTableF_Func() {
	(this->*opTableF[opcode & 0x00FF])();


}

void Chip8::setFunctionTable() {
	
	opTable0[0x0] = &Chip8::cls;
	opTable0[0xE] = &Chip8::ret;
	
	opTable8[0x0] = &Chip8::LD_Vx_Vy;
	opTable8[0x1] = &Chip8::OR_Vx_Vy;
	opTable8[0x2] = &Chip8::AND_Vx_Vy;
	opTable8[0x3] = &Chip8::XOR_Vx_Vy;
	opTable8[0x4] = &Chip8::ADD_Vx_Vy;
	opTable8[0x5] = &Chip8::SUB_Vx_Vy;
	opTable8[0x6] = &Chip8::SHR_Vx_Vy;
	opTable8[0x7] = &Chip8::SUBN_Vx_Vy;
	opTable8[0xE] = &Chip8::SHL_Vx_Vy;
	
	opTableE[0x9E] = &Chip8::SKP_Vx;
	opTableE[0xA1] = &Chip8::SKNP_Vx;
	
	opTableF[0x07] = &Chip8::LD_Vx_DT;
	opTableF[0x0A] = &Chip8::LD_Vx_K;
	opTableF[0x15] = &Chip8::LD_DT_Vx;
	opTableF[0x18] = &Chip8::LD_ST_Vx;
	opTableF[0x1E] = &Chip8::ADD_I_Vx;
	opTableF[0x29] = &Chip8::LD_F_Vx;
	opTableF[0x33] = &Chip8::LD_B_Vx;
	opTableF[0x55] = &Chip8::LD_I_Vx;
	opTableF[0x65] = &Chip8::LD_Vx_I;
	
	opTable[0x0] = &Chip8::opTable0_Func;
	opTable[0x1] = &Chip8::JPaddr;
	opTable[0x2] = &Chip8::CALLaddr;
	opTable[0x3] = &Chip8::SE_Vx_byte;
	opTable[0x4] = &Chip8::SNE_Vx_byte;
	opTable[0x5] = &Chip8::SE_Vx_Vy;
	opTable[0x6] = &Chip8::LD_Vx_byte;
	opTable[0x7] = &Chip8::ADD_Vx_byte;
	opTable[0x8] = &Chip8::opTable8_Func;
	opTable[0x9] = &Chip8::SNE_Vx_Vy;
	opTable[0xA] = &Chip8::LD_I_addr;
	opTable[0xB] = &Chip8::JP_V0_addr;
	opTable[0xC] = &Chip8::RND_Vx_byte;
	opTable[0xD] = &Chip8::DRW_Vx_Vy_nibble;
	opTable[0xE] = &Chip8::opTableE_Func;
	opTable[0xF] = &Chip8::opTableF_Func;
}
void Chip8::cls() {
	//Clear the display.

	for (unsigned int y = 0; y != std::end(*display) - std::begin(*display); ++y) {
	
		for (unsigned int x = 0; x != std::end(display) - std::begin(display); ++x)
			display[x][y] = false;
	}

	updateScreen = true;
	//std::cout << "screen is clean\n";
}
void Chip8::ret() {
	// Return from a subroutine.
	// The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
	
	if (!usingStack) {
		throw std::runtime_error("Cannot use the RET opcode while not in a function!");
		}
	
	pc = stack[sp];
	
	if (sp == 0x0) usingStack = false;
	else --sp;
	
	//std::cout << "return from function\n";
}

void Chip8::JPaddr() {
	// The interpreter sets the program counter to nnn.

	pc = opcode & 0x0FFF;

}

void Chip8::CALLaddr() {
	// The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.

	if(!usingStack) {
		usingStack = true;
		stack[sp] = pc;
		
	}
	else {
		if (sp == 15) {throw std::runtime_error("Stack overflow!");}
		stack[++sp] = pc;
	}
	
	pc = opcode & 0x0FFF;
}

void Chip8::SE_Vx_byte() {
	//Skip next instruction if Vx = kk.
	//The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
	
	if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) pc += 2;
	
	if(pc > 0xFFF) throw std::runtime_error("The program counter points outside the memory space!");
		
	
	
	
}

void Chip8::SNE_Vx_byte() {
/*4xkk - SNE Vx, byte
Skip next instruction if Vx != kk.
*/

	if (v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) pc += 2;
	
	if(pc > 0xFFF) throw std::runtime_error("The program counter points outside the memory space!");

}

void Chip8::SE_Vx_Vy() {
/*5xy0 - Skip next instruction if Vx = Vy.
The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
*/

	uint8_t vx, vy;
	
	vx = v[(opcode & 0x0F00) >> 8];
	vy = v[(opcode & 0x00F0) >> 4];
	
	if(vx == vy) {
	
	pc += 2;
	
	if (pc > 0xFFF) throw std::runtime_error ("The program counter points outside the memory space!");
	
	
	}
}
void Chip8::LD_Vx_byte() {
/*6xkk - Set Vx = kk.

The interpreter puts the value kk into register Vx.
*/

	v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);

	
}	
void Chip8::ADD_Vx_byte() {
/*7xkk - Set Vx = Vx + kk.

Adds the value kk to the value of register Vx, then stores the result in Vx. 
*/

	v[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
}	
void Chip8::LD_Vx_Vy() {
/*8xy0 - Set Vx = Vy.

Stores the value of register Vy in register Vx.
*/

	v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4]; 
}
void Chip8::OR_Vx_Vy() {
/*8xy1 - Set Vx = Vx OR Vy.

Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0. 
*/

	v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];

}
void Chip8::AND_Vx_Vy()	{
/*8xy2 - Set Vx = Vx AND Vy.

Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0. 
*/

	v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
	
}
void Chip8::XOR_Vx_Vy()	{
/*8xy3 - Set Vx = Vx XOR Vy.

Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0. 
*/

	v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];

}
void Chip8::ADD_Vx_Vy()	{
/*8xy4 - Set Vx = Vx + Vy, set VF = carry.

The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
*/

	uint16_t temp = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4];

	v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
	v[0xF] = temp > 0xFF ? 1 : 0; 

}
void Chip8::SUB_Vx_Vy()	{
/*8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.

If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
*/

	v[0xF] = v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4] ? 1 : 0;
	
	v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
	
}
void Chip8::SHR_Vx_Vy()	{
/*8xy6 - Set Vx = Vx SHR 1.

If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
*/

	
	v[0xF] = (v[(opcode & 0x0F00) >> 8] & 0x1) == 0x1 ? 1 : 0;
	
	v[(opcode & 0x0F00) >> 8] >>= 1;
	

}

void Chip8::SUBN_Vx_Vy() {
/*
8xy7 - SUBN Vx, Vy
Set Vx = Vy - Vx, set VF = NOT borrow.

If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
*/

	uint8_t vx, vy;
	vx = v[(opcode & 0x0F00) >> 8];
	vy = v[(opcode & 0x00F0) >> 4];
	
	//std::cout << "vx = " << std::hex << static_cast<int>(vx) << "\tvy = " << std::hex << static_cast<int>(vy) << "\t vy - vx = ";
	
	v[0xF] = vy > vx ? 1 : 0;
	
	v[(opcode & 0x00F0) >> 4] -= v[(opcode & 0x0F00) >> 8];
	v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
	/*
	if (v[0xF] == 1)
		v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
	else
		v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
	*/
		
	//std::cout << std::hex << static_cast<int>(v[(opcode & 0x0F00) >> 8]) << std::endl; 
	
}

void Chip8::SHL_Vx_Vy() {
/*
8xyE - SHL Vx {, Vy}
Set Vx = Vx SHL 1.

If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
*/

	//check most-significant bit of Vx
	bool mostSignificantBit = (v[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
	v[0xF] = mostSignificantBit ? 1 : 0;
	
	//multiply Vx by 2
	 
	 v[(opcode & 0x0F00) >> 8] <<= 1;


}

void Chip8::SNE_Vx_Vy() {
/*
9xy0 - SNE Vx, Vy
Skip next instruction if Vx != Vy.

The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
*/

	uint8_t vx, vy;
	vx = v[(opcode & 0x0F00) >> 8];
	vy = v[(opcode & 0x00F0) >> 4];

	pc += vx != vy ? 2 : 0;

}

void Chip8::LD_I_addr() {
/*
Annn - LD I, addr
Set I = nnn.

The value of register I is set to nnn.
*/

	i = opcode & 0x0FFF;

}

void Chip8::JP_V0_addr() {
/*
Bnnn - JP V0, addr
Jump to location nnn + V0.

The program counter is set to nnn plus the value of V0.
*/

	pc = (opcode & 0x0FFF) + v[0x0];
	
	if (pc > 0xFFF)
		throw std::runtime_error("The PC points outside the memory space!");


}

void Chip8::RND_Vx_byte() {
/*
Cxkk - RND Vx, byte
Set Vx = random byte AND kk.

The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
*/
	
	auto distribution = std::uniform_int_distribution<uint8_t> (0,255);
	
	v[(opcode & 0x0F00) >> 8] = distribution(generator) & (opcode & 0x00FF);

}

void Chip8::DRW_Vx_Vy_nibble() {
/*
Dxyn - DRW Vx, Vy, nibble
Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
*/
	updateScreen = true;
	uint8_t vx, vy;
	v[0xF] = 0;
	vx = v[(opcode & 0x0F00) >> 8];
	vy = v[(opcode & 0x00F0) >> 4];
	
	uint8_t maxHeight = opcode & 0x000F;
	
	for(unsigned int height = 0x0; height != maxHeight ; ++height) {
		
		//load sprite row
		uint8_t spriteRow = memory[i + height];
		
		//std::cout << std::hex << static_cast<int>(spriteRow) << std::endl;
		
		uint8_t bitPointer = 0b10000000;
		unsigned int bitShiftCounter = 7;
		
		uint8_t xDraw, yDraw;
		
		for(unsigned int xRelative = 0; xRelative != 8 ; ++xRelative) {
			xDraw = vx + xRelative;
			yDraw = vy + height;
			
			if(xDraw > 63)
				xDraw -= 64;
			
			if(yDraw > 31)
				yDraw -= 32;
			
			if(v[0xF] == 0 && display[xDraw][yDraw] == true)
				v[0xF] = display[xDraw][yDraw] ^ (spriteRow & bitPointer) >> bitShiftCounter ? 0 : 1;
				
			//std::cout << std::boolalpha << static_cast<bool>(spriteRow & bitPointer >> bitShiftCounter) << std::endl;
			
			
			display[xDraw][yDraw] ^= (spriteRow & bitPointer) >> bitShiftCounter;
			
			bitPointer >>= 1;
			--bitShiftCounter;
			
		}
	
	}

}

void Chip8::SKP_Vx() {
/*
Ex9E - SKP Vx
Skip next instruction if key with the value of Vx is pressed.

Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
*/

	pc += keypad[v[(opcode & 0x0F00) >> 8]] ? 2 : 0;

}

void Chip8::SKNP_Vx() {
/*
ExA1 - SKNP Vx
Skip next instruction if key with the value of Vx is not pressed.

Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
*/

	pc += (keypad[v[(opcode & 0x0F00) >> 8]] == false) ? 2 : 0;

}

void Chip8::LD_Vx_DT() {
/*
Fx07 - LD Vx, DT
Set Vx = delay timer value.

The value of DT is placed into Vx.
*/

	v[(opcode & 0x0F00) >> 8] = delayRegister;


}

void Chip8::LD_Vx_K() {
/*
Fx0A - LD Vx, K
Wait for a key press, store the value of the key in Vx.

All execution stops until a key is pressed, then the value of that key is stored in Vx.
*/

	bool halt = true;

	for (unsigned int it = 0; it != 16; ++it) {
		
		if(keypad[it]) {
			v[(opcode & 0x0F00) >> 8] = it;
			halt = false;
		}
	
	}

	pc -= halt ? 2 : 0;

}

void Chip8::LD_DT_Vx() {
/*
Fx15 - LD DT, Vx
Set delay timer = Vx.

DT is set equal to the value of Vx.
*/

	delayRegister = v[(opcode & 0x0F00) >> 8];
	startDelay = std::chrono::high_resolution_clock().now();
	remainderDecimals = 0;
}

void Chip8::LD_ST_Vx() {
/*
Fx18 - LD ST, Vx
Set sound timer = Vx.

ST is set equal to the value of Vx.
*/
	sound = true;
	soundRegister = v[(opcode & 0x0F00) >> 8];
	startDelaySound = std::chrono::high_resolution_clock().now();
	remainderDecimalsSound = 0;
	

}

void Chip8::ADD_I_Vx() {
/*
Fx1E - ADD I, Vx

Set I = I + Vx.
The values of I and Vx are added, and the results are stored in I.
*/

	i += v[(opcode & 0x0F00) >> 8];
	
	if(i > 0x0FFF)
		throw std::runtime_error("I points to an invalid memory location!");
		
}

void Chip8::LD_F_Vx() {
/*
Fx29 - LD F, Vx

Set I = location of sprite for digit Vx.
The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
*/

	i = (v[(opcode & 0x0F00) >> 8] * 5) + Font_StartAddress;
}

void Chip8::LD_B_Vx() {
/*
Fx33 - LD B, Vx

Store BCD representation of Vx in memory locations I, I+1, and I+2.
The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
*/

	uint8_t value = v[(opcode & 0x0F00) >> 8];
	
	memory[i + 2] = value % 10;
	value /= 10;
	
	memory[i + 1] = value % 10;
	value /= 10;
	
	memory[i] = value % 10;
}

void Chip8::LD_I_Vx() {
/*
Fx55 - LD [I], Vx

Store registers V0 through Vx in memory starting at location I.
The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
*/

	if ( (i +  ((opcode & 0x0F00) >> 8)) > 0x0FFF)
		throw std::runtime_error("I points to an invalid memory location!");

	for(unsigned int it = 0; it <= ((opcode & 0x0F00) >> 8); ++it) {
		
		memory[i + it] = v[it];
	
	}
}

void Chip8::LD_Vx_I() {
/*
Fx65 - LD Vx, [I]

Read registers V0 through Vx from memory starting at location I.
The interpreter reads values from memory starting at location I into registers V0 through Vx.
*/

	for(unsigned int it = 0; it <= ((opcode & 0x0F00) >> 8); ++it) {
		
		v[it] = memory[i + it];
	
	}

}

void Chip8::unknownOpcode() {
	std::cout << "unkown opcode: " << std::hex << static_cast<int>(opcode) << std::endl; 
}
#endif //CHIP8
