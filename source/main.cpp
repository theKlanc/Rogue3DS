#ifdef _WIN32
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "graphicsPC.h"
#else
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include "graphics3ds.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include "common.h"
#include "gameMap.h"

using namespace std;

void cameraOperation(entity player, int &cameraX, int &cameraY, const int mapHeight, const int mapWidth) {
	if (player.posX < mapWidth - 10) { if (cameraX + 14 < player.posX) { cameraX++; } }
	if (player.posX > 9) { if (cameraX + 10 > player.posX) { cameraX--; } }
	if (player.posY < mapHeight - 6) { if (cameraY + 8 < player.posY) { cameraY++; } }
	if (player.posY > 5) { if (cameraY + 6 > player.posY) { cameraY--; } }
}

class gameMain {
private:
#ifdef _WIN32
	sf::RenderWindow window;
	sf::Texture overlay;
#else
	u32 kDown;
	u32 kUp;
	u32 kHeld;
	sf2d_texture *overlay;
#endif

	gameMap map;
	graphics graphicsObj;
	entity* player;
	bool showGrid = 0;

	long loop = 0;
	bool autoJump = 0;
	int jumped = 0;
	void exitGame() {
#ifndef _WIN32
		
		csndExit();
		sf2d_fini();
		gfxExit();
		hidExit();
		aptExit();
		srvExit();
#endif

		exit(1);
	}
	void moveEntity(entity &currentEntity, direction dir, bool autojump = false) {
		switch (dir) {
		case DOWN:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ - 1) == 0) {
				currentEntity.posZ--;
			}
			break;
		case UP:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0) {
				currentEntity.posZ++;
			}
			break;
		case FRONT:
			if (currentEntity.posY > 8) {
				if (map.simpleCollision(currentEntity.posX, currentEntity.posY - 1, currentEntity.posZ) == 0) {
					currentEntity.posY--;
				}
				else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX, currentEntity.posY - 1, currentEntity.posZ + 1) == 0) {
					currentEntity.posY--;
					currentEntity.posZ++;
				}
			}
			else {
				cout << "BARRERA INVISBLE LOKO" << endl;
			}
			break;
		case BACK:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY + 1, currentEntity.posZ) == 0) {
				currentEntity.posY++;
			}
			else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX, currentEntity.posY + 1, currentEntity.posZ + 1) == 0) {
				currentEntity.posY++;
				currentEntity.posZ++;
			}
			break;
		case LEFT:
			if (currentEntity.posX > 16) {
				if (map.simpleCollision(currentEntity.posX - 1, currentEntity.posY, currentEntity.posZ) == 0) {
					currentEntity.posX--;
				}
				else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX - 1, currentEntity.posY, currentEntity.posZ + 1) == 0) {
					currentEntity.posX--;
					currentEntity.posZ++;
				}
			}
			else {
				cout << "BARRERA INVISBLE LOKO" << endl;
			}
			break;
		case RIGHT:
			if (map.simpleCollision(currentEntity.posX + 1, currentEntity.posY, currentEntity.posZ) == 0) {
				currentEntity.posX++;
			}
			else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX + 1, currentEntity.posY, currentEntity.posZ + 1) == 0) {
				currentEntity.posX++;
				currentEntity.posZ++;
			}
			break;
		}
	}

	void updateEntity(entity &currentEntity) {
		if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ - 1) == 0) {
			//cout<< "que es faci la gravetat " << endl;
			currentEntity.posZ--;
			if (currentEntity.posZ < 0) {
				cout << "has caigut del mon, capoll" << endl;
				exitGame();
			}
		}
		switch (currentEntity.type) {
		case NPC:
			break;
		}
	}
	void updateEntities() {
		for (int i = 0; i < ENTITY_LIST_SIZE && map.entityList[i].posX >= 0; i++)
		{
			updateEntity(map.entityList[i]);
		}
	}
	void handleInput() {
#ifdef _WIN32
	
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			exitGame();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			autoJump = !autoJump;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			showGrid = 1;
		}
		else showGrid = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			moveEntity(*player, RIGHT, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			moveEntity(*player, LEFT, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			moveEntity(*player, FRONT, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			moveEntity(*player, BACK, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (!jumped) {
				moveEntity(*player, UP, autoJump);
				moveEntity(*player, UP, autoJump);
				jumped = 2;
			}
		}
#else

	hidScanInput();
	kDown = hidKeysDown();
	kHeld = hidKeysHeld();
	if (kDown & KEY_START) {
		exitGame();
	}
	if (kDown & KEY_X) {
		autoJump = !autoJump;
	}
	if (kHeld & KEY_SELECT) {
		showGrid = 1;
	}
	else showGrid = 0;
	if (kHeld & KEY_RIGHT) {
		moveEntity(*player, RIGHT, autoJump);
	}
	if (kHeld & KEY_LEFT) {
		moveEntity(*player, LEFT, autoJump);
	}
	if (kHeld & KEY_UP) {
		moveEntity(*player, FRONT, autoJump);
	}
	if (kHeld & KEY_DOWN) {
		moveEntity(*player, BACK, autoJump);
	}
	if (kHeld & KEY_A) {
		if (!jumped) {
			moveEntity(*player, UP, autoJump);
			moveEntity(*player, UP, autoJump);
			jumped = 2;
		}
	}
#endif
	}
void gameLoop() {
	//cout<< "loop." << loop;

	point3D playerPos;
	playerPos.x = player->posX;
	playerPos.y = player->posY;
	playerPos.z = player->posZ;
	if (loop % 10 == 0) {
		handleInput();
		updateEntities();
	}
	map.loadNewChunk(playerPos);
	if (jumped > 0) { jumped--; }

	//cout<< player->posX << ' ' << player->posY << ' ' << player->posZ << endl;
#ifdef _WIN32
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
	}
#else 
	
	sf2d_swapbuffers();
#endif


	//processar input
	//refrescar coses carregades
	//dibuixar

	graphicsObj.drawFrame();
}
public:
	void gameCore() {
#ifdef _WIN32
		window.create(sf::VideoMode(400, 240), "RoguePC");
		sf::Music music;
		music.openFromFile("data/sounds/bgm/wilderness.ogg");
		music.play();
#endif
		//gameMap map;
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


		//overlay = sfil_load_PNG_file("data/sprites/overlay.png", SF2D_PLACE_RAM);
		
		player = &map.entityList[0];
		graphicsObj.edit(map, *player);
		string saveName = "default";
		ifstream general;
		general.open("saves/" + saveName + "/general.txt");
		if (!general.is_open()) {
			//cout<< "couldn't open file: " << ("saves/" + saveName + "/general.txt") << endl;
		}
		string playerSprite, playerName;
		general >> playerName >> player->spriteName >> player->posX >> player->posY >> player->posZ;
		//cout<< playerName << endl;
		point3D playerPos;
		//cout<< player->posX << endl;
		playerPos.x = player->posX;
		playerPos.y = player->posY;
		playerPos.z = player->posZ;
		//loadmap
		for (int i = 0; i < CHUNK_NUM; i++) {
			map.loadNewChunk(playerPos);
		}
		map.loadTerrainTable();
		graphicsObj.reloadTextures();
		//audio_load("data/sounds/bgm/wilderness.raw"); //[N3DS] only 
		while (1) {
			gameLoop();
			loop++;
		}
	}
};

int main()
{
#ifndef _WIN32
	srvInit();
	aptInit();
	hidInit();
	srand(time(NULL));
	sf2d_init();
	consoleInit(GFX_BOTTOM, NULL);
	csndInit();
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
#endif
	srand(time(NULL));
	gameMain gameMain1;
	gameMain1.gameCore();
#ifndef _WIN32
	
	csndExit();


		// Exit
		//map.freeAllTextures();

		sf2d_fini();

	gfxExit();

	hidExit();

	aptExit();

	srvExit();
#endif
	return 0;
}