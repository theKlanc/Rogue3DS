 /*
 TODO:eficientitzar la carrega i descarrega de chunks, trobar el tamany òptim i el nombre de chunks òptims
      vigilar k el thread no intenti currar massa
	  separar les entities, la entitylist,el gamemain i les fisiques
	  afegir creacio de directoris quan no els pugui trobar
	  arreglar les inputs i seprarales
	  fer menu
	  afegir UI
	  ficar mode creatiu
	  implementar el format .qb 
 */
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include "graphics.h"
#include "sound.h"
#include "3ds/thread.h"

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

//void chunkLoader(u32 arg) {
//	while (1) {
//		threadArg1 *temporal = (threadArg1*)arg;
//		temporal->map->loadNewChunk(*temporal->player);
//		if (*temporal->exit == 1) {
//			threadExit(0);
//		}
//		svcSleepThread(50000000);
//	}
//}

class gameMain {
private:
	bool exitBool = 0;
	u32 kDown;
	u32 kUp;
	u32 kHeld;
	sf2d_texture *overlay;

	int block;
	gameMap map;
	graphics graphicsObj;
	sound soundObj;

	bool showGrid = 0;

	long loop = 0;
	bool autoJump = 0;
	int jumped = 0;
	void exitGame() {

		exitBool = 1;
		cout << "exiting..." << endl;
		point3D temp;
		temp.x = 9999999;
		temp.y = 9999999;
		temp.z = 9999999;
		svcSleepThread(1000000000);
		map.freeAllChunks();
		
		csndExit();
		sf2d_fini();
		gfxExit();
		hidExit();
		aptExit();
		srvExit();


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
		if (map.simpleCollision(c) == 0 && currentEntity.fly == 0) {
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



		if (kDown & KEY_START) {
			exitGame();
		}
		if (kDown & KEY_X) {
			player->fly = !player->fly;
		}
		if (kDown & KEY_R) {
			block++;
		}
		if (kDown & KEY_L) {
			block--;
		}
		if (kHeld & KEY_Y || kDown & KEY_Y) {
			map.putBlock(block, player->pos);
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
		if (kHeld & KEY_B) {
			if (player->fly) {
				moveEntity(*player, DOWN, autoJump);
			}
		}
		if (kHeld & KEY_A) {
			if (player->fly) {
				moveEntity(*player, UP, autoJump);
			}
			/*if (!jumped) {
				moveEntity(*player, UP, autoJump);
				moveEntity(*player, UP, autoJump);
				jumped = 2;
			}*/
		}

	}
	void gameLoop() {
		//cout<< "loop." << loop;
		
		
		if (loop % 13 == 0) {
			hidScanInput();
			kDown = hidKeysDown();
			kHeld = hidKeysHeld();
			handleInput();
			updateEntities();
		}if (loop % 15 == 0) {
			soundObj.update();
		}
		

		if (jumped > 0) { jumped--; }

		//cout<< player->posX << ' ' << player->posY << ' ' << player->posZ << endl;

		sf2d_swapbuffers();



		//processar input
		//refrescar coses carregades
		//dibuixar

		graphicsObj.drawFrame();
	}
public:
	entity* player;
	void gameCore() {
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
		block = 0;
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
		static threadArg1 temporal;
		temporal.map = &map;
		temporal.player = &player->pos;
		temporal.exit = &exitBool;
		soundObj.playFromFile("/mau5.ogg");
		//threadCreate((ThreadFunc)gameMap::chunkLoader, (&temporal), 5000, 0x3F, 0, false);
		map.ayy((u32)&temporal);

		while (1) {
			gameLoop();
			loop++;
		}
	}
};

int main()
{

	srvInit();
	aptInit();
	hidInit();
	srand(time(NULL));
	sf2d_init();
	consoleInit(GFX_BOTTOM, NULL);
	csndInit();
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));

	srand(time(NULL));
	gameMain gameMain1;
	gameMain1.gameCore();


	csndExit();


	// Exit
	//map.freeAllTextures();

	sf2d_fini();

	gfxExit();

	hidExit();

	aptExit();

	srvExit();

	return 0;
}