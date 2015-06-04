#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <math.h>
#include <sf2d.h>
#include <time.h>

extern "C" {
	extern const struct {
		unsigned int 	 width;
		unsigned int 	 height;
		unsigned int 	 bytes_per_pixel;
		unsigned char	 pixel_data[];
	} dwarf_img;

	extern const struct {
		unsigned int 	 width;
		unsigned int 	 height;
		unsigned int 	 bytes_per_pixel;
		unsigned char	 pixel_data[];
	} wall_img;

	extern const struct {
		unsigned int 	 width;
		unsigned int 	 height;
		unsigned int 	 bytes_per_pixel;
		unsigned char	 pixel_data[];
	} field_img;
}
struct entity{
	sf2d_texture *spriteData;
	int spriteHeight;
	int spriteWidth;
	int posX;
	int posY;
	bool tangible;
	bool divertit;
};

void cameraOperation(entity player, int &cameraX, int &cameraY, const int mapHeight, const int mapWidth){
	if (player.posX < mapWidth - 10){ if (cameraX + 14 < player.posX){ cameraX++; } }
	if (player.posX > 9){ if (cameraX + 10>player.posX){ cameraX--; } }
	if (player.posY < mapHeight - 6){ if (cameraY + 8 < player.posY){ cameraY++; } }
	if (player.posY > 5){ if (cameraY + 6>player.posY){ cameraY--; } }
}

int main()
{
	// Inits
	srvInit();
	aptInit();
	hidInit(NULL);

	srand(time(NULL));

	sf2d_init();
	//gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	sf2d_texture *tex1 = sf2d_create_texture(dwarf_img.width, dwarf_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_fill_texture_from_RGBA8(tex1, dwarf_img.pixel_data, dwarf_img.width, dwarf_img.height);
	sf2d_texture_tile32(tex1);

	sf2d_texture *tex2 = sf2d_create_texture(wall_img.width, wall_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_fill_texture_from_RGBA8(tex2, wall_img.pixel_data, wall_img.width, wall_img.height);
	sf2d_texture_tile32(tex2);

	sf2d_texture *tex3 = sf2d_create_texture(field_img.width, field_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);
	sf2d_fill_texture_from_RGBA8(tex3, field_img.pixel_data, field_img.width, field_img.height);
	sf2d_texture_tile32(tex3);

	u32 kDown;
	u32 kHeld;
	u32 kUp;
	int cameraY = 0;
	int cameraX = 0;
	int temp = 0;
	int npcN = 0;
	const int mapHeight = 1000;
	const int mapWidth = 1000;
	static entity* map[mapWidth][mapHeight];

	entity player;
	player.spriteData = tex1;
	player.spriteHeight = 16;
	player.spriteWidth = 16;
	player.tangible = 1;
	player.divertit = 0;

	entity NPC1;
	NPC1.spriteData = tex1;
	NPC1.spriteHeight = 16;
	NPC1.spriteWidth = 16;
	NPC1.tangible = 1;
	NPC1.posX = 3;
	NPC1.posY = 3;
	entity field;
	field.spriteData = tex3;
	field.spriteHeight = 16;
	field.spriteWidth = 16;
	field.tangible = 0;
	field.divertit = 1;

	entity wall;
	wall.spriteData = tex2;
	wall.spriteHeight = 16;
	wall.spriteWidth = 16;
	wall.tangible = 1;
	wall.divertit = 0;

	for (int i = 0; i != mapHeight; i++){
		for (int j = 0; j != mapWidth; j++){
			map[j][i] = &wall;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++){
		for (int j = 1; j != mapWidth - 1; j++){
			map[j][i] = &field;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++){
		for (int j = 1; j != mapWidth - 1; j++){
			if (rand() % 20 == 0){ map[j][i] = &wall; }
		}
	}

	player.posX = 5;
	player.posY = 5;
	map[5][5] = &player;

	while (aptMainLoop()){
		//PREPARATIUS

		sf2d_swapbuffers();
		//INPUTS

		if (temp == 0){ hidScanInput(); }
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		kUp = hidKeysUp();

		if (kDown & KEY_START){
			int option = 0;
			bool loop = 1;
			bool changed = 0;
			printf(">>BACK");
			printf("\n");
			printf("EXIT");
			while (loop==1){
				if (kDown & KEY_UP){
					option--;
					changed = 1;
				}
					
				if (kDown & KEY_DOWN){
					option++;
					changed = 1;
				}
				if (changed == 1){
					changed = 0;
					consoleClear();
					if (option == 0){
						printf(">>BACK");
						printf("\n");
						printf("EXIT");
					}
					if (option == 1){
						printf("BACK");
						printf("\n");
						printf(">>EXIT");
					}

				}
				if (kDown & KEY_A){
					if (option == 0){ loop = 0; }
					if (option == 1){ break; }
				}
			}
		}
		if (temp == 0){
			if (kHeld & KEY_UP){
				if (map[player.posX][player.posY - 1]->tangible == 0){
					map[player.posX][player.posY] = &field;
					player.posY--;
					map[player.posX][player.posY] = &player;
				}
			}
			if (kHeld & KEY_LEFT){
				if (map[player.posX - 1][player.posY]->tangible == 0){
					map[player.posX][player.posY] = &field;
					player.posX--;
					map[player.posX][player.posY] = &player;
				}
			}
			if (kHeld & KEY_RIGHT){
				if (map[player.posX + 1][player.posY]->tangible == 0){
					map[player.posX][player.posY] = &field;
					player.posX++;
					map[player.posX][player.posY] = &player;
				}
			}
			if (kHeld & KEY_DOWN){
				if (map[player.posX][player.posY + 1]->tangible == 0){
					map[player.posX][player.posY] = &field;
					player.posY++;
					map[player.posX][player.posY] = &player;

				}
			}
		}

		int rng1 = rand() % 4;
		if (temp == 0 & rand() % 3 == 0){ //si el tick es 0, una de cada 3 vegades(probabilisticament)
			switch (rng1){
			case 2:
				if (map[NPC1.posX][NPC1.posY - 1]->tangible == 0){
					map[NPC1.posX][NPC1.posY] = &field;
					NPC1.posY--;
					map[NPC1.posX][NPC1.posY] = &NPC1;
				}
				break;

			case 1:
				if (map[NPC1.posX - 1][NPC1.posY]->tangible == 0){
					map[NPC1.posX][NPC1.posY] = &field;
					NPC1.posX--;
					map[NPC1.posX][NPC1.posY] = &NPC1;
				}
				break;

			case 3:
				if (map[NPC1.posX + 1][NPC1.posY]->tangible == 0){
					map[NPC1.posX][NPC1.posY] = &field;
					NPC1.posX++;
					map[NPC1.posX][NPC1.posY] = &NPC1;
				}
				break;

			case 0:
				if (map[NPC1.posX][NPC1.posY + 1]->tangible == 0){
					map[NPC1.posX][NPC1.posY] = &field;
					NPC1.posY++;
					map[NPC1.posX][NPC1.posY] = &NPC1;
				}
				break;
			}
		}

		if (temp > 0){ temp--; }
		else { temp = 4; }
		//OUTPUT
		cameraOperation(player, cameraX, cameraY, mapHeight, mapWidth); //Moure la camera

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		for (int i = 0; i != 15; i++){
			for (int j = 0; j != 25; j++){
				sf2d_draw_texture(map[j + cameraX][i + cameraY]->spriteData, j * 16, i * 16);
			}
		}
		sf2d_end_frame();

	}

	// Exit
	sf2d_free_texture(tex1);
	sf2d_free_texture(tex2);
	sf2d_free_texture(tex3);

	sf2d_fini();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();

	// Return to hbmenu
	return 0;
}