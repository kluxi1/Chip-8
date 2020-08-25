#include <iostream>
#include <chrono>

#include "chip8.h"
#include "platform.h"

bool oneCycle(decltype(std::chrono::high_resolution_clock().now())& timer1, decltype(std::chrono::high_resolution_clock().now())& timer2, const double& frequency) {
	
	timer2 = std::chrono::high_resolution_clock().now();
	
	double dt = std::chrono::duration<double, std::chrono::milliseconds::period>(timer2 - timer1).count();
	
	if ( dt >= ((1/frequency)*1000)) {
		timer1 = std::chrono::high_resolution_clock().now();
		return true;
	
	} 
	
	return false;

}

int main(int argc, char* argv[]) {

	double freq = 500;
	decltype(std::chrono::high_resolution_clock().now()) start = std::chrono::high_resolution_clock().now(), lastTick;
	
	if(argc <= 1) {
		std::cerr << "Specify a game name as an argument please!\n"; 
		return 0;
	}
	

	Chip8 cpu;
	cpu.init();
	cpu.loadRom(argv[1]);

	platform window(argv[1], cpu, 20);
	
	
	while(window.windowIsOpen()) {

		if(oneCycle(start, lastTick, freq)) {

			cpu.cycle();
		
			if(cpu.getUpdateScreen()) {
				window.render();
				cpu.resetUpdateScreen();
			}

		}

		if(cpu.getSoundSignal()) {
		
			window.playSound(true);
			
		
		} else {
			window.playSound(false);
		}

		window.readKeyboard();

		window.readEvent();
	
	}

	return 0;
}
