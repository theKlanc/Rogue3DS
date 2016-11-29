#include <iostream>
#include <string>
#include <fstream>
#include "../include/core.h"
#include "../include/entityx/entityx.h"
#include "../include/inputSystem.h"
#include "../include/movementSystem.h"
#include "../include/components.h"
#include "../include/gameCore.h"
#include "../include/AISystem.h"


using namespace std;
namespace ex = entityx;

void gameCore::gameLoop()
{
	HI::updateHID();
	kDown = kDown | HI::getKeysDown();
	kHeld = kHeld | HI::getKeysHeld();
	kUp = kUp | HI::getKeysUp();
	if (tick % 12 == 0) {
		EntityWorld->systems.update_all(0);
		kDown = HI::getKeysDown();
		kHeld = HI::getKeysHeld();
		kUp = HI::getKeysUp();
	}
	else  HI::waitForVBlank();
	if (kDown & HI::HI_KEY_START) {
		exitBool = true;
	}
	//cout<< player->posX << ' ' << player->posY << ' ' << player->posZ << endl;




	//processar input
	//refrescar coses carregades
	//dibuixar


}

void gameCore::gameLaunch()
{

	// Inits

	//demanar arxius a carregar i ficar un menu o halgo

	//

	/*
	ifstream index;
	index.open("saves/index.txt");
	int saveNum;
	index >> saveNum;
	string saveList[saveNum];
	for (int i=0;i<saveNum;i++){
	index >> saveList[i];
	}
	index.close();
	//select which one
	saveName = selected;
	*/
	
	HI::setBackgroundColor(RGBA8(0, 0, 0, 0));
	ifstream general;
	general.open(HI::getSavesPath() + saveName + "/general.txt");
	if (!general.is_open()) {
		cout << "couldn't open file: " << (HI::getSavesPath()+ saveName + "/general.txt") << endl;
	}
	string playerSprite = "player.png", playerName;
	string cacota;
	general >> cacota >> playerPos->x >> playerPos->y >> playerPos->z;
	general.close();
	playerPos->z =1 + FLOOR_HEIGHT + 100 * (1+ map->noiseObj.GetNoise(playerPos->x, playerPos->y)/2);
	puts("LMAOOOO");
	map->addPlayer(playerPos);
	for (int i = 0; i < CHUNK_NUM; i++) {
		map->loadNewChunk();
	}
	map->loadTerrainTable();
	EntityWorld->systems.add<inputSystem>(&kDown,&kUp,&kHeld);
	EntityWorld->systems.add<movementSystem>(map);
	EntityWorld->systems.add<graphicsSystem>(map, playerPos);
	EntityWorld->systems.add<AISystem>();
	EntityWorld->systems.configure();
	entityx::Entity test = EntityWorld->entities.create();
	test.assign<Position>(*playerPos);
	point3D caca;
	caca.x++;
	caca.y++;
	caca.z++;
	test.assign<Velocity>(caca);
	test.assign<Player>(playerPos);
	test.assign<FixedSprite>("player.png");
	point3D dogPos = *playerPos;
	dogPos.x += 4;
	dogPos.y += 6;

	entityx::Entity doggo = EntityWorld->entities.create();
	doggo.assign<AIFollower>(playerPos,3);
	doggo.assign<Velocity>(caca);
	doggo.assign<Position>(dogPos);
	doggo.assign<FixedSprite>("doggo.png");	
	//soundObj.playFromFile(HI::getDataPath()+"sounds/bgm/wilderness.ogg");
	map->startChunkLoader();
	tick = 0;
	while (HI::aptMainLoop() && !exitBool) {
		gameLoop();
		tick++;
	}

	string generalFile = HI::getSavesPath() + saveName + "/general.txt";
	std::remove(generalFile.c_str());
	
	ofstream generalO(generalFile);
	generalO << playerName << endl << playerPos->x << endl << playerPos->y << endl << playerPos->z << endl;
	generalO.close();
	map->exit();
}

void gameCore::createSavefile(string saveName)
{
	HI::createDir(HI::getSavesPath() + saveName + "/");
	HI::createDir(HI::getSavesPath() + saveName + "/chunks/");
	HI::copyFile(HI::getDataPath() + "gameData/defaultSavefile/general.txt", HI::getSavesPath() + saveName + "/general.txt");
	HI::copyFile(HI::getDataPath() + "gameData/defaultSavefile/terrainTable.txt", HI::getSavesPath() + saveName + "/terrainTable.txt");	  
}

void gameCore::loadSavefile(string saveID)
{
	map = new gameMap(saveID);
	saveName = saveID;
}

gameCore::gameCore()
{
	EntityWorld = new ex::EntityX();
	kDown = 0;
	kUp = 0;
	kHeld = 0;
	playerPos = new point3D;
	map = nullptr;
	exitBool = false;
	tick = 0;
}

gameCore::~gameCore()
{
}

void gameCore::exit()
{
	delete map;
	delete EntityWorld;
	delete playerPos;
}

void gameCore::gameMenu()
{
	struct button
	{
		HI::HITexture pressed;
		HI::HITexture free;
		int sizeX;
		int sizeY;
		int posX;
		int posY;
		bool state;
		button()
		{
			pressed = nullptr;
			free = nullptr;
			sizeX = 0;
			sizeY = 0;
			posX = 0;
			posY = 0;
			state = false;
		}
		void update(point2D touch, unsigned int kDown, unsigned int kHeld)
		{
			if ((kDown | kHeld) & HI::HI_KEY_TOUCH)
			{
				if (touch.x >= posX && touch.x <= posX + sizeX && touch.y >= posY && touch.y <= posY + sizeY)
				{
					state = true;
				}
				else state = false;
			}
			else state = false;
		}
		HI::HITexture getTexture() const
		{
			if (state) return pressed;
			else return free;
		}
	};
	/*
	<Create save>
	<Load save>
	<Options>
	<Quit>
	*/
	HI::setBackgroundColor(RGBA8(53, 159, 35, 0xFF));
	HI::HITexture topImage = HI::loadPngFile(HI::getDataPath()+"sprites/menu_top.png");
	HI::startFrame(HardwareInterface::SCREEN_TOP);
	HI::drawTexture(topImage, 0, 0);
	HI::endFrame();
	HI::swapBuffers();

	HI::HITexture unpressedButton = HI::loadPngFile(HI::getDataPath()+"sprites/unpressed_button.png");
	HI::HITexture  pressedButton = HI::loadPngFile(HI::getDataPath()+"sprites/pressed_button.png");

	button newGame;
	newGame.posX = 85;
	newGame.posY = 60;
	newGame.sizeX = 150;
	newGame.sizeY = 40;
	newGame.pressed = pressedButton;
	newGame.free = unpressedButton;
	newGame.state = false;

	button loadGame;
	loadGame.posX = 85;
	loadGame.posY = 140;
	loadGame.sizeX = 150;
	loadGame.sizeY = 40;
	loadGame.pressed = pressedButton;
	loadGame.free = unpressedButton;
	loadGame.state = false;

	point2D touch;

	while (HI::aptMainLoop()) {
		HI::updateHID();

		HI::updateTouch(touch);
		kDown = HI::getKeysDown();
		kHeld = HI::getKeysHeld();
		kUp = HI::getKeysUp();
		HI::startFrame(HardwareInterface::SCREEN_BOT);

		HI::drawTexture(newGame.getTexture(), newGame.posX, newGame.posY);
		HI::drawTexture(loadGame.getTexture(), loadGame.posX, loadGame.posY);

		HI::endFrame();
		HI::startFrame(HardwareInterface::SCREEN_TOP);
		HI::drawTexture(topImage, 0, 0);
		HI::endFrame();

		HI::swapBuffers();
		if (kDown& HI::HI_KEY_START)return;
		
		if (HI::getPlatform() == HI::PLATFORM_PC || HI::getPlatform() == HI::PLATFORM_PSVITA || newGame.state && (kUp & HI::HI_KEY_TOUCH))
		{
			createSavefile("default");
			loadSavefile("default");
			gameLaunch();
			HI::freeTexture(topImage);
			HI::freeTexture(newGame.getTexture());
			HI::freeTexture(loadGame.getTexture());
			return;
		}
		if (loadGame.state && (kUp & HI::HI_KEY_TOUCH))
		{
			loadSavefile("default");
			gameLaunch();
			HI::freeTexture(topImage);
			HI::freeTexture(newGame.getTexture());
			HI::freeTexture(loadGame.getTexture());
			return;
		}

		loadGame.update(touch, kDown, kHeld);
		newGame.update(touch, kDown, kHeld);

	}
	HI::freeTexture(topImage);
	HI::freeTexture(newGame.getTexture());
	HI::freeTexture(loadGame.getTexture());
}
