#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "functions.c"
#include "dwarf_bin.h"
#include "field_bin.h"
#include "wall_bin.h"

struct entity{
	u8* spriteData;
	int spriteHeight;
	int spriteWidth;
	int posX;
	int posY;
	bool tangible;
};

void cameraOperation(entity player, int &cameraX, int &cameraY){
	if (player.posX < 190){ if (cameraX + 14 < player.posX){ cameraX++; } }
	if (player.posX > 9){ if (cameraX + 10>player.posX){ cameraX--; } }
	if (player.posY < 194){ if (cameraY + 8 < player.posY){ cameraY++; } }
	if (player.posY > 5){ if (cameraY + 6>player.posY){ cameraY--; } }
}

int main()
{
	// Initializations
	srvInit();        // services
	aptInit();        // applets
	hidInit(NULL);    // input
	gfxInitDefault();//initialize GPU
	consoleInit(GFX_BOTTOM, NULL);
	u32 kDown;        // keys down
	u32 kHeld;        // keys pressed
	u32 kUp;          // keys up
	int cameraY = 0;
	int cameraX = 0;
	int temp = 0;
	int npcN = 0;
	static short map[200][200];
	static entity* list[64];

	entity player;
	player.spriteData = (u8*)dwarf_bin;
	player.spriteHeight = 16;
	player.spriteWidth = 16;
	player.tangible = 1;

	entity NPC1;
	NPC1.spriteData = (u8*)dwarf_bin;
	NPC1.spriteHeight = 16;
	NPC1.spriteWidth = 16;
	NPC1.tangible = 1;
	NPC1.posX = 3;
	NPC1.posY= 3;

	entity field;
	field.spriteData = (u8*)field_bin;
	field.spriteHeight = 16;
	field.spriteWidth = 16;
	field.tangible = 0;

	entity wall;
	wall.spriteData = (u8*)wall_bin;
	wall.spriteHeight = 16;
	wall.spriteWidth = 16;
	wall.tangible = 1;

	list[0] = &field;
	list[1] = &player;
	list[2] = &wall;
	list[3] = &NPC1;

	for (int i = 0; i != 200; i++){
		for (int j = 0; j != 200; j++){
			map[j][i] = 2;
		}
	}
	for (int i = 1; i != 199; i++){
		for (int j = 1; j != 199; j++){
			map[j][i] = 0;
		}
	}

	player.posX = 5;
	player.posY = 5;
	// Main loop
	while (aptMainLoop()){
		//PREPARATIUS
		// Wait for next frame
		gspWaitForVBlank();

		//INPUTS
		// Read which buttons are currently pressed or not
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		kUp = hidKeysUp();

		// If START button is pressed, break loop and quit

		if (temp == 0){
			if (kHeld & KEY_DOWN){
				if (list[map[player.posX][player.posY - 1]]->tangible == 0){
					map[player.posX][player.posY] = 0;
					player.posY--;
					temp = 2;
				}
			}
			if (kHeld & KEY_LEFT){
				if (list[map[player.posX - 1][player.posY]]->tangible == 0){
					map[player.posX][player.posY] = 0;
					player.posX--;
					temp = 2;
				}
			}
			if (kHeld & KEY_RIGHT){
				if (list[map[player.posX + 1][player.posY]]->tangible == 0){
					map[player.posX][player.posY] = 0;
					player.posX++;
					temp = 2;
				}
			}
			if (kHeld & KEY_UP){
				if (list[map[player.posX][player.posY + 1]]->tangible == 0){
					map[player.posX][player.posY] = 0;
					player.posY++;
					temp = 2;
				}
			}
		}
		
		//CODI
		if (npcN == 2){
			if (list[map[NPC1.posX][NPC1.posY - 1]]->tangible == 0){
				map[NPC1.posX][NPC1.posY] = 0;
				NPC1.posY--;			
			}
		}
		else if (npcN == 1){
			if (list[map[NPC1.posX - 1][NPC1.posY]]->tangible == 0){
				map[NPC1.posX][NPC1.posY] = 0;
				NPC1.posX--;				
			}
		}
		else if (npcN == 3){
			if (list[map[NPC1.posX + 1][NPC1.posY]]->tangible == 0){
				map[NPC1.posX][NPC1.posY] = 0;
				NPC1.posX++;
				
			}
		}
		else if (npcN==0){
			if (list[map[NPC1.posX][NPC1.posY + 1]]->tangible == 0){
				map[NPC1.posX][NPC1.posY] = 0;
				NPC1.posY++;
			}
		}
		
		if (npcN ==3){
			npcN = 0;
		}
		else { npcN++; }
		std::cout << "NPC1.posX==" << NPC1.posX << std::endl << "NPC1.posY==" << NPC1.posY << std::endl;
		if (temp > 0){ temp--; }
		//OUTPUT
		map[player.posX][player.posY] = 1;
		map[NPC1.posX][NPC1.posY] = 3;
		cameraOperation(player, cameraX, cameraY);

		for (int i = 0; i != 15; i++){
			for (int j = 0; j != 25; j++){
				gfxDrawSprite(GFX_TOP, GFX_LEFT, list[map[j + cameraX][i + cameraY]]->spriteData, 16, 16, i * 16, j * 16);
			}
		}
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	// Exit
	gfxExit();
	hidExit();
	aptExit();
	srvExit();

	// Return to hbmenu
	return 0;
}