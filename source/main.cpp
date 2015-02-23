#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

void gfxDrawSprite(gfxScreen_t screen, gfx3dSide_t side, u8* spriteData, u16 width, u16 height, s16 x, s16 y) {
	//This function includes documantation so you might be able to figure out what the function is doing, you don't need to understand this to use it!
	if (!spriteData)return; //check if the function has sprite data, if not stop!

	u16 fbWidth, fbHeight; //set variables for width and height
	u8* fbAdr = gfxGetFramebuffer(screen, side, &fbWidth, &fbHeight); //get framebuffer for the screen and side used.

	if (x + width < 0 || x >= fbWidth)return; //check invalid x cords
	if (y + height < 0 || y >= fbHeight)return; //check invalid y cords

	u16 xOffset = 0, yOffset = 0; //set offset for x and y
	u16 widthDrawn = width, heightDrawn = height; //set width/height vars that for drawing

	if (x < 0) xOffset = -x; //use offset
	if (y < 0) yOffset = -y; //use offset
	if (x + width >= fbWidth) widthDrawn = fbWidth - x;
	if (y + height >= fbHeight) heightDrawn = fbHeight - y;
	widthDrawn -= xOffset;
	heightDrawn -= yOffset;

	int j;
	for (j = yOffset; j < yOffset + heightDrawn; j++) { //for loop for drawing image
		memcpy(&fbAdr[((x + xOffset) + (y + j) * fbWidth) * 3], &spriteData[((xOffset + j) * width) * 3], widthDrawn * 3); //copy imagedata into memory
	}
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
	static entity* map[2000][2000];      //Around 2300 (some corruption) and 4600 (well) works, also below 50.
	
	entity player;
		player.spriteData = 1;
		player.spriteHeight = 16;
		player.spriteWidth = 16;
		player.tangible = 1;

	entity field;
		field.spriteData = 2;
		field.spriteHeight = 16;
		field.spriteWidth = 16;
		field.tangible = 0;

	
	
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
		if (kHeld & KEY_UP){
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
		if (kHeld & KEY_DOWN){
			if (map[player.posX][player.posY+1]->tangible == 0){
				player.posY++;
			}
		}
		
		
		map[player.posX][player.posY] = &player;

	
		
		
		for (int i = 0; i != 15; i++){

			for (int j = 0; j != 25; j++){
				gfxDrawSprite(GFX_TOP, GFX_LEFT, map[cameraX + j][cameraY + i]->spriteData, 16, 16, 240 - 16 * i - 16, 16 * j);
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