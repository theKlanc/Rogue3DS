#pragma once
#include "State_Base.h"

struct button {
	HI::HITexture pressed;
	HI::HITexture free;
	int sizeX;
	int sizeY;
	int posX;
	int posY;
	bool state;
	button() {
		pressed = nullptr;
		free = nullptr;
		sizeX = 0;
		sizeY = 0;
		posX = 0;
		posY = 0;
		state = false;
	}
	void update(point2D touch, unsigned int kDown, unsigned int kHeld) {
		if ((kDown | kHeld) & HI::HI_KEY_TOUCH) {
			if (touch.x >= posX && touch.x <= posX + sizeX && touch.y >= posY && touch.y <= posY + sizeY) {
				state = true;
			}
			else state = false;
		}
		else state = false;
	}
	HI::HITexture getTexture() const {
		if (state) return pressed;
		else return free;
	}
};

namespace State {
	class MainMenu : public State_Base {
	public:
		MainMenu(gameCore& application);
		~MainMenu();
		void input() override;
		void update(float dt) override;
		void draw() override;
		static void createSavefile(string name);

	private:
		button loadGame;
		button newGame;
		HI::HITexture topImage;
	};
}

