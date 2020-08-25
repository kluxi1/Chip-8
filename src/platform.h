#ifndef PLATFORM_H
#define PLATFORM_H

#include "chip8.h"

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

class platform {

	public: 
		platform(const std::string& t, Chip8& c, const unsigned int& sc) : gameTitle(t +  " | Chip8 emulator - Nasser Fathi"), cpu(c), scale(sc) 
		{
			window.create(sf::VideoMode((64*sc), (32*sc)), gameTitle, sf::Style::Titlebar | sf::Style::Close);
			
			raw = new sf::Int16[SAMPLES];
			
			//load sine wave and set sound settings
			const unsigned AMPLITUDE = 30000;
			const double TWO_PI = 6.28318;
			const double increment = 440./44100;
			double x = 0;
			for (unsigned i = 0; i < SAMPLES; i++) {
				raw[i] = AMPLITUDE * sin(x*TWO_PI);
				x += increment;
			}
			
			if (!Buffer.loadFromSamples(raw, SAMPLES, 1, SAMPLE_RATE)) {
				std::cerr << "Loading failed!" << std::endl;

			}
			
			Sound.setBuffer(Buffer);
			Sound.setLoop(true);
		}
		
		
		bool windowIsOpen() {return window.isOpen();}
		void readEvent();
		void render();
		void readKeyboard();
		
		void playSound(bool toggle)
		{
		
			if(toggle && !soundIsOn) {
				Sound.play();
				soundIsOn = true;
			} else if(!toggle) {
				Sound.stop();
				soundIsOn = false;
			}
			
		}
		
		
	private:
		
		
		const std::string gameTitle;
		Chip8& cpu;
		const unsigned int scale;
		
		sf::RenderWindow window;
		
		//sound
		const unsigned SAMPLES = 44100;
		const unsigned SAMPLE_RATE = 44100;
	
		sf::Int16* raw;
	
		sf::SoundBuffer Buffer;
		
		sf::Sound Sound;
		bool soundIsOn = false;
		

};

void platform::readEvent(){

	sf::Event event;

	while(window.pollEvent(event)) {
	
		switch(event.type) {
		
			case sf::Event::Closed:
				window.close();
				break;
			/*case sf::Event::KeyPressed:
				readKeyboard();
				break;*/
			default:
				break;
		
		}
		
	
	}

}

void platform::render(){
	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;
	
	image.create(64,32,sf::Color::Black);
	
	for(unsigned int ity = 0; ity!= 32; ++ity) {
	
		for(unsigned int itx = 0; itx != 64; ++itx) {
		
			if(cpu.getPixel(itx,ity))
				image.setPixel(itx,ity,sf::Color::White);
		
		}
	
	
		
	
	}

	
	texture.loadFromImage(image);
	sprite.setTexture(texture);
	sprite.setScale(scale, scale);
	window.clear();
	

	window.draw(sprite);
	window.display();
}

void platform::readKeyboard(){

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
    		cpu.setKeypad(0x0, true);
	} else {
	
		cpu.setKeypad(0x0, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
    		cpu.setKeypad(0x1, true);
	} else {
	
		cpu.setKeypad(0x1, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
    		cpu.setKeypad(0x2, true);
	} else {
	
		cpu.setKeypad(0x2, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
    		cpu.setKeypad(0x3, true);
	} else {
	
		cpu.setKeypad(0x3, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
	{
    		cpu.setKeypad(0x4, true);
	} else {
	
		cpu.setKeypad(0x4, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
    		cpu.setKeypad(0x5, true);
	} else {
	
		cpu.setKeypad(0x5, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
    		cpu.setKeypad(0x6, true);
	} else {
	
		cpu.setKeypad(0x6, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
    		cpu.setKeypad(0x7, true);
	} else {
	
		cpu.setKeypad(0x7, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
    		cpu.setKeypad(0x8, true);
	} else {
	
		cpu.setKeypad(0x8, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
	{
    		cpu.setKeypad(0x9, true);
	} else {
	
		cpu.setKeypad(0x9, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
    		cpu.setKeypad(0xA, true);
	} else {
	
		cpu.setKeypad(0xA, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
    		cpu.setKeypad(0xB, true);
	} else {
	
		cpu.setKeypad(0xB, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
    		cpu.setKeypad(0xC, true);
	} else {
	
		cpu.setKeypad(0xC, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
	{
    		cpu.setKeypad(0xD, true);
	} else {
	
		cpu.setKeypad(0xD, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
	{
    		cpu.setKeypad(0xE, true);
	} else {
	
		cpu.setKeypad(0xE, false);
	
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
	{
    		cpu.setKeypad(0xF, true);
	} else {
	
		cpu.setKeypad(0xF, false);
	
	}

}

#endif //PLATFORM_H
