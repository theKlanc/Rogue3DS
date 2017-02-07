#include "../../include/states/state_playing.h"
#include "../../include/gameCore.h"
#include "../../include/states/state_main_menu.h"

namespace State {
	MainMenu::MainMenu(gameCore& application)
		: State_Base(application) {

		HI::setBackgroundColor(RGBA8(53, 159, 35, 0xFF));
		topImage = HI::loadPngFile(HI::getDataPath() + "sprites/menu_top.png");
		HI::startFrame(HardwareInterface::SCREEN_TOP);
		HI::drawTexture(topImage, 0, 0);
		HI::endFrame();
		HI::swapBuffers();

		HI::HITexture unpressedButton = HI::loadPngFile(HI::getDataPath() + "sprites/unpressed_button.png");
		HI::HITexture  pressedButton = HI::loadPngFile(HI::getDataPath() + "sprites/pressed_button.png");


		newGame.posX = 85;
		newGame.posY = 60;
		newGame.sizeX = 150;
		newGame.sizeY = 40;
		newGame.pressed = pressedButton;
		newGame.free = unpressedButton;
		newGame.state = false;


		loadGame.posX = 85;
		loadGame.posY = 140;
		loadGame.sizeX = 150;
		loadGame.sizeY = 40;
		loadGame.pressed = pressedButton;
		loadGame.free = unpressedButton;
		loadGame.state = false;
	}

	MainMenu::~MainMenu() {
		HI::freeTexture(topImage);
		HI::freeTexture(newGame.getTexture());
		HI::freeTexture(loadGame.getTexture());
	}

	void MainMenu::input() {
		HI::updateHID();
		HI::updateTouch(touch);
		kDown = HI::getKeysDown();
		kHeld = HI::getKeysHeld();
		kUp = HI::getKeysUp();



		if (newGame.state && (kUp & HI::HI_KEY_TOUCH)) {
			createSavefile("default");
			core->pushState(std::make_unique<State::Playing>(*core));
			return;
		}
		else if (loadGame.state && (kUp & HI::HI_KEY_TOUCH)) {
			core->pushState(std::make_unique<State::Playing>(*core));
			return;
		}
		else if (kDown & HI::HI_KEY_START)core->quit();
	}

	void MainMenu::update(float dt) {
		loadGame.update(touch, kDown, kHeld);
		newGame.update(touch, kDown, kHeld);
	}

	void MainMenu::draw() {
		HI::startFrame(HardwareInterface::SCREEN_TOP);
		HI::drawTexture(topImage, 0, 0);
		HI::endFrame();
		HI::startFrame(HardwareInterface::SCREEN_BOT);

		HI::drawTexture(newGame.getTexture(), newGame.posX, newGame.posY);
		HI::drawTexture(loadGame.getTexture(), loadGame.posX, loadGame.posY);

		HI::endFrame();
		HI::swapBuffers();
	}

	void MainMenu::createSavefile(string name) {
		HI::createDir(HI::getSavesPath() + name + "/");
		HI::createDir(HI::getSavesPath() + name + "/chunks/");
		HI::copyFile(HI::getDataPath() + "gameData/defaultSavefile/general.txt", HI::getSavesPath() + name + "/general.txt");
		HI::copyFile(HI::getDataPath() + "gameData/defaultSavefile/terrainTable.txt", HI::getSavesPath() + name + "/terrainTable.txt");
	}
}
