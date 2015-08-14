/*
TO-DO
1-crear llista d entities
2-^-crear custom entities(instancies?)-^(no se si es podra)
4-afegir menu principal
3-separar la creacio de mapa i guardar mapa (guardat dinamic)      /  / RELACIONAT
5-afegir suport per transicio de mapes seamless                   /  /  RELACIONAT
6-gestio de memoria                    /  / JUNT
7-modularitat en càrrega de textures  /  /  JUNT
8-afegir events
9-afegir combat
10-millorar el so i afegir sfx

99-MP



*/



#include <3ds.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <sf2d.h>
#include <time.h>
//test
using namespace std;

extern "C"{
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
	bool posZ;
	bool solid;
};

void cameraOperation(entity player, int &cameraX, int &cameraY, const int mapHeight, const int mapWidth){
	if (player.posX < mapWidth - 10){ if (cameraX + 14 < player.posX){ cameraX++; } }
	if (player.posX > 9){ if (cameraX + 10 > player.posX){ cameraX--; } }
	if (player.posY < mapHeight - 6){ if (cameraY + 8 < player.posY){ cameraY++; } }
	if (player.posY > 5){ if (cameraY + 6 > player.posY){ cameraY--; } }
}

//Audio Stuff WIP
u8* buffer;
u32 size;

static void audio_load(const char *audio);
static void audio_stop(void);


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

	sf2d_texture *tex1 = sf2d_create_texture(dwarf_img.width, dwarf_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);   //s ha de fer bbbbbbb
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
	char temp = 0;
	bool stop = 0;
	const short mapHeight = 100;
	const short mapWidth = 100;
	short map[mapWidth][mapHeight]; //necessita memalloc
	entity* list[10];

	entity air;
	air.solid = 1;
	list[0] = &air;

	entity player;
	player.spriteData = tex1;
	player.spriteHeight = 16;
	player.spriteWidth = 16;
	player.solid = 1;
	list[9] = &player;
	
	entity field;
	field.spriteData = tex3;
	field.spriteHeight = 16;
	field.spriteWidth = 16;
	field.solid = 0;
	list[1] = &field;

	entity wall;
	wall.spriteData = tex2;
	wall.spriteHeight = 16;
	wall.spriteWidth = 16;
	wall.solid = 1;
	list[2] = &wall;

	for (int i = 0; i != mapHeight; i++){
		for (int j = 0; j != mapWidth; j++){
			map[j][i] = 2;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++){
		for (int j = 1; j != mapWidth - 1; j++){
			map[j][i] = 1;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++){
		for (int j = 1; j != mapWidth - 1; j++){
			if (rand() % 20 == 0){ map[j][i] = 2; }
		}
	}

	player.posX = 5;
	player.posY = 5;
	map[5][5] = 9;

	csndInit();//start Audio Lib
	audio_load("audio/original_raw.bin");

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
			while (loop){
				hidScanInput();
				kDown = hidKeysDown();
				if (kDown & KEY_UP){
					if (option > 0){
						option--;
						changed = 1;
					}
				}

				if (kDown & KEY_DOWN){
					if (option < 1){
						option++;
						changed = 1;
					}
				}
				if (changed){
					changed = 0;
					consoleClear();
					if (option == 0){
						printf(">>BACK");
						printf("\n");
						printf("EXIT");
					}
					if (option){
						printf("BACK");
						printf("\n");
						printf(">>EXIT");
					}

				}
				if (kDown & KEY_A){
					if (!option){ consoleClear(); break; }
					if (option){ stop = 1; consoleClear(); break; }
				}
				gfxSwapBuffers();
				gfxFlushBuffers();

			}
		}
		if (!temp){
			if (kHeld & KEY_UP){
				if (!list[map[player.posX][player.posY - 1]]->solid){
					map[player.posX][player.posY] = 1;
					player.posY--;
					map[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_LEFT){
				if (!list[map[player.posX - 1][player.posY]]->solid){
					map[player.posX][player.posY] = 1;
					player.posX--;
					map[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_RIGHT){
				if (!list[map[player.posX + 1][player.posY]]->solid){
					map[player.posX][player.posY] = 1;
					player.posX++;
					map[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_DOWN){
				if (!list[map[player.posX][player.posY + 1]]->solid){
					map[player.posX][player.posY] = 1;
					player.posY++;
					map[player.posX][player.posY] = 9;

				}
			}
			if (kHeld & KEY_A){
				if (map[player.posX + 1][player.posY] == 2){
					map[player.posX + 1][player.posY] = 1;
				}
				else{
					map[player.posX + 1][player.posY] = 2;
				}
			}
			if (kHeld & KEY_B){
				if (map[player.posX][player.posY + 1] == 2){
					map[player.posX][player.posY + 1] = 1;
				}
				else{
					map[player.posX][player.posY + 1] = 2;
				}
			}
			if (kHeld & KEY_Y){
				if (map[player.posX - 1][player.posY] == 2){
					map[player.posX - 1][player.posY] = 1;
				}
				else{
					map[player.posX - 1][player.posY] = 2;
				}
			}
			if (kHeld & KEY_X){
				if (map[player.posX][player.posY - 1] == 2){
					map[player.posX][player.posY - 1] = 1;
				}
				else{
					map[player.posX][player.posY - 1] = 2;
				}
			}

		}

		int rng1 = rand() % 4;

		if (temp > 0){ temp--; }
		else { temp = 4; }
		//OUTPUT
		cameraOperation(player, cameraX, cameraY, mapHeight, mapWidth); //Moure la camera

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		for (int i = 0; i != 15; i++){
			for (int j = 0; j != 25; j++){
				sf2d_draw_texture(list[map[j + cameraX][i + cameraY]]->spriteData, j * 16, i * 16);
			}
		}
		sf2d_end_frame();
		if (stop){ break; }
	}

	audio_stop();
	audio_stop();
	csndExit();
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


void audio_load(const char *audio){

	FILE *file = fopen(audio, "rb");

	// seek to end of file
	fseek(file, 0, SEEK_END);

	// file pointer tells us the size
	off_t size = ftell(file);

	// seek back to start
	fseek(file, 0, SEEK_SET);

	//allocate a buffer
	buffer = linearAlloc(size);

	//read contents !
	off_t bytesRead = fread(buffer, 1, size, file);
	//u8 test = &buffer;

	//close the file because we like being nice and tidy
	fclose(file);

	csndPlaySound(8, SOUND_FORMAT_16BIT | SOUND_REPEAT, 48000, 1, 0, buffer, buffer, size);
	linearFree(buffer);
}
void audio_stop(void){
	csndExecCmds(true);
	CSND_SetPlayState(0x8, 0);
	memset(buffer, 0, size);
	GSPGPU_FlushDataCache(NULL, buffer, size);
	linearFree(buffer);
}
