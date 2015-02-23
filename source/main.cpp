#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
	static short map[200][200];
	static entity* list[64];
	
	entity player;
		player.spriteData =(u8*)dwarf_bin;
		player.spriteHeight = 16;
		player.spriteWidth = 16;
		player.tangible = 1;

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
		if (kDown & KEY_START){
			break;
		}
		if (kHeld & KEY_DOWN){
			if (list[map[player.posX][player.posY-1]]->tangible == 0){
			map[player.posX][player.posY] = 0;
			player.posY--;
			}
		}
		if (kHeld & KEY_LEFT){
			if (list[map[player.posX - 1][player.posY]]->tangible == 0){
			map[player.posX][player.posY] = 0; 
			player.posX--;
			}
		}
		if (kHeld & KEY_RIGHT){
			if (list[map[player.posX + 1][player.posY]]->tangible == 0){
			map[player.posX][player.posY] = 0; 
			player.posX++;
			}
		}
		if (kHeld & KEY_UP){
			if (list[map[player.posX][player.posY+1]]->tangible == 0){
			map[player.posX][player.posY] = 0; 
			player.posY++;
			}
		}		
		map[player.posX][player.posY] = 1;
		//CODI


		
		//OUTPUT
		
		for (int i = 0; i != 15; i++){

			for (int j = 0; j != 25; j++){
				gfxDrawSprite(GFX_TOP, GFX_LEFT,list[map[j][i]]->spriteData, 16, 16, 16 * i, 16 * j);
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