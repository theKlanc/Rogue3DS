#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "functions.c"
#include "dwarf_bin.h"
#include "field_bin.h"


struct entity{
	int spriteData;
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
	static entity* map[50][50];      //Around 2300 (some corruption) and 4600 (well) works, also below 50.
	
	entity player;
		player.spriteData = 1;
		player.spriteHeight = 16;
		player.spriteWidth = 16;
		player.tangible = 1;

	entity field;
		field.spriteData = 0;
		field.spriteHeight = 16;
		field.spriteWidth = 16;
		field.tangible = 0;

		u8* sprites[32];
		sprites[0] = (u8*)field_bin;
		sprites[1] = (u8*)dwarf_bin;
	player.posX = 0;
	player.posY = 0;
	// Main loop
	while (aptMainLoop()){
		for (int y = 0; y != 15; y++){              
			for (int x = 0; x != 25; x++){
				map[x][y] = &field;         
			}
		}
		
		
		

		
				
		// Wait for next frame
		gspWaitForVBlank();
		
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
			if (map[player.posX][player.posY-1]->tangible == 0){
				player.posY--;
			}
		}
		if (kHeld & KEY_LEFT){
			if (map[player.posX - 1][player.posY]->tangible == 0){
				player.posX--;
			}
		}
		if (kHeld & KEY_RIGHT){
			if (map[player.posX + 1][player.posY]->tangible == 0){
				player.posX++;
			}
		}
		if (kHeld & KEY_UP){
			if (map[player.posX][player.posY+1]->tangible == 0){
				player.posY++;
			}
		}
		
		
		map[player.posX][player.posY] = &player;

	
		
		
		for (int i = 0; i != 15; i++){

			for (int j = 0; j != 25; j++){
				gfxDrawSprite(GFX_TOP, GFX_LEFT, sprites[map[cameraX + j][cameraY + i]->spriteData], 16, 16, 16 * i, 16 * j);
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