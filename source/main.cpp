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
#include "3ds/thread.h"
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
	if (player.pos.x < mapWidth - 10) { if (cameraX + 14 < player.pos.x) { cameraX++; } }
	if (player.pos.x > 9) { if (cameraX + 10 > player.pos.x) { cameraX--; } }
	if (player.pos.y < mapHeight - 6) { if (cameraY + 8 < player.pos.y) { cameraY++; } }
	if (player.pos.y > 5) { if (cameraY + 6 > player.pos.y) { cameraY--; } }
}

struct temp {
	gameMap* map;
	point3D* player;
};
void chunkLoader(u32 arg) {
	while (1) {
		temp *temporal = (temp*)arg;
		temporal->map->loadNewChunk(*temporal->player);
	}
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
			if (map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z - 1) == 0) {
				currentEntity.pos.z--;
			}
			break;
		case UP:
			if (map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) == 0) {
				currentEntity.pos.z++;
			}
			break;
		case FRONT:
			if (currentEntity.pos.y > 8) {
				if (map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y - 1, currentEntity.pos.z) == 0) {
					currentEntity.pos.y--;
				}
				else if (autojump && map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) == 0 && map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y - 1, currentEntity.pos.z + 1) == 0) {
					currentEntity.pos.y--;
					currentEntity.pos.z++;
				}
			}
			else {
				cout << "BARRERA INVISBLE LOKO" << endl;
			}
			break;
		case BACK:
			if (map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y + 1, currentEntity.pos.z) == 0) {
				currentEntity.pos.y++;
			}
			else if (autojump && map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) == 0 && map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y + 1, currentEntity.pos.z + 1) == 0) {
				currentEntity.pos.y++;
				currentEntity.pos.z++;
			}
			break;
		case LEFT:
			if (currentEntity.pos.x > 16) {
				if (map.simpleCollision(currentEntity.pos.x - 1, currentEntity.pos.y, currentEntity.pos.z) == 0) {
					currentEntity.pos.x--;
				}
				else if (autojump && map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) == 0 && map.simpleCollision(currentEntity.pos.x - 1, currentEntity.pos.y, currentEntity.pos.z + 1) == 0) {
					currentEntity.pos.x--;
					currentEntity.pos.z++;
				}
			}
			else {
				cout << "BARRERA INVISBLE LOKO" << endl;
			}
			break;
		case RIGHT:
			if (map.simpleCollision(currentEntity.pos.x + 1, currentEntity.pos.y, currentEntity.pos.z) == 0) {
				currentEntity.pos.x++;
			}
			else if (autojump && map.simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) == 0 && map.simpleCollision(currentEntity.pos.x + 1, currentEntity.pos.y, currentEntity.pos.z + 1) == 0) {
				currentEntity.pos.x++;
				currentEntity.pos.z++;
			}
			break;
		}
	}

	void updateEntity(entity &currentEntity) {
		point3D c = currentEntity.pos;
		c.z--;
		if (map.simpleCollision(c) == 0) {
			//cout<< "que es faci la gravetat " << endl;
			currentEntity.pos.z--;
			if (currentEntity.pos.z < 0) {
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
		for (int i = 0; i < ENTITY_LIST_SIZE && map.entityList[i].pos.x >= 0; i++)
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
		if (loop % 13 == 0) {
			handleInput();
			updateEntities();
			//cout << "Player height: " << player->pos.z << endl;
		}
		/*if (loop % 115 == 0) {
			map.loadNewChunk(player->pos);
		}*/

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
	entity* player;
	void gameCore() {
#ifdef _WIN32
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
		general >> playerName >> player->spriteName >> player->pos.x >> player->pos.y >> player->pos.z;
		//cout<< playerName << endl;
		//loadmap
		for (int i = 0; i < CHUNK_NUM; i++) {
			map.loadNewChunk(player->pos);
		}
		map.loadTerrainTable();
		graphicsObj.reloadTextures();
		static temp temporal;
		temporal.map = &map;
		temporal.player = &player->pos;

		threadCreate((ThreadFunc)(void*)chunkLoader, (&temporal), 5000, 0x3F, 0, 1);
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