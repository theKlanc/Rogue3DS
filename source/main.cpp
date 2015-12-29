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

extern "C" {
	extern const struct {
		unsigned int 	 width;
		unsigned int 	 height;
		unsigned int 	 bytes_per_pixel;
		unsigned char	 pixel_data[];
	} dwarf_img, wall_img, field_img;
}
struct entity {
	sf2d_texture *spriteData;
	int posX;
	int posY;
	int posZ;
	bool solid;
};

struct terrain {
	sf2d_texture *spriteData;
	bool solid;
};
struct point3D {
	int x;
	int y;
	int z;
};


class map {
private:
	static char terrainMap[3][3][3][100][100][100];
	entity entityList[1000]; //tractat a part del terreny un a un
	terrain terrainList[1000]; //terrain és derivat de entity
	point3D mapIndex[27]; //indica quin bloc de terreny hi ha a cada posició
	int blocX = 0, blocY = 0, blocZ = 0;
public:
	void emplenarLlistaWIP() {
		terrainList[0].solid = 0;
		terrainList[1].solid = 1;
		//[0].spriteData = tex0;
		//terrainList[1].spriteData = tex1;


	}
	void emplenarWIP() {
		for (int x = 0; x != 3; x++) {
			for (int y = 0; y != 3; y++) {
				for (int z = 0; z != 3; z++) {
					for (int i = 0; i <= 99; i++) {
						for (int j = 0; i <= 99; j++) {
							for (int k = 0; k <= 1; k++) {
								terrainMap[x][y][z][i][j][49 + k] = 1;
							}
						}
					}
					for (int i = 1; i != 99; i++) {
						for (int j = 1; i != 99; j++) {
							terrainMap[x][y][z][i][j][50] = 0;
						}
					}
				}
			}
		}

	}
	//void loadChunk(int chunkX,int chunkY,int chunkZ, int fillX, int fillY, int fillZ)
	/*bool collision(int x, int y, int z) {
	//ultra-wip falta abstracció pel mapa
	}*/
}map;

void cameraOperation(entity player, int &cameraX, int &cameraY, const int mapHeight, const int mapWidth) {
	if (player.posX < mapWidth - 10) { if (cameraX + 14 < player.posX) { cameraX++; } }
	if (player.posX > 9) { if (cameraX + 10 > player.posX) { cameraX--; } }
	if (player.posY < mapHeight - 6) { if (cameraY + 8 < player.posY) { cameraY++; } }
	if (player.posY > 5) { if (cameraY + 6 > player.posY) { cameraY--; } }
}


static u8* buffer;
static u32 size;

void audio_load(const char *audio);
void audio_stop(void);


int main()
{
	// Inits
	srvInit();
	aptInit();
	hidInit();

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
	const short mapHeight = 2000;
	const short mapWidth = 2000;
	static short map[mapWidth][mapHeight]; //necessita memalloc
	static entity* list[10];

	entity air;
	air.solid = 1;
	list[0] = &air;

	entity player;
	player.spriteData = tex1;
	player.solid = 1;
	list[9] = &player;

	entity field;
	field.spriteData = tex3;
	field.solid = 0;
	list[1] = &field;

	entity wall;
	wall.spriteData = tex2;
	wall.solid = 1;
	list[2] = &wall;

	for (int i = 0; i != mapHeight; i++) {
		for (int j = 0; j != mapWidth; j++) {
			map[j][i] = 2;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++) {
		for (int j = 1; j != mapWidth - 1; j++) {
			map[j][i] = 1;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++) {
		for (int j = 1; j != mapWidth - 1; j++) {
			if (rand() % 20 == 0) { map[j][i] = 2; }
		}
	}

	player.posX = 5;
	player.posY = 5;
	map[5][5] = 9;

	csndInit();//start Audio Lib
	audio_load("audio/original_raw.bin");

	while (aptMainLoop()) {
		//PREPARATIUS

		sf2d_swapbuffers();
		//INPUTS

		if (temp == 0) { hidScanInput(); }
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		kUp = hidKeysUp();


		if (!temp) {
			if (kHeld & KEY_UP) {
				if (!list[map[player.posX][player.posY - 1]]->solid) {
					map[player.posX][player.posY] = 1;
					player.posY--;
					map[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_LEFT) {
				if (!list[map[player.posX - 1][player.posY]]->solid) {
					map[player.posX][player.posY] = 1;
					player.posX--;
					map[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_RIGHT) {
				if (!list[map[player.posX + 1][player.posY]]->solid) {
					map[player.posX][player.posY] = 1;
					player.posX++;
					map[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_DOWN) {
				if (!list[map[player.posX][player.posY + 1]]->solid) {
					map[player.posX][player.posY] = 1;
					player.posY++;
					map[player.posX][player.posY] = 9;

				}
			}
			if (kDown & KEY_START) {
				stop = 1;
			}
		}

		int rng1 = rand() % 4;

		if (temp > 0) { temp--; }
		else { temp = 4; }
		//OUTPUT
		cameraOperation(player, cameraX, cameraY, mapHeight, mapWidth); //Moure la camera

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		for (int i = 0; i != 15; i++) {
			for (int j = 0; j != 25; j++) {
				sf2d_draw_texture(list[map[j + cameraX][i + cameraY]]->spriteData, j * 16, i * 16);
			}
		}
		sf2d_end_frame();
		if (stop) { break; }
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


void audio_load(const char *audio) {

	FILE *file = fopen(audio, "rb");

	// seek to end of file
	fseek(file, 0, SEEK_END);

	// file pointer tells us the size
	off_t size = ftell(file);

	// seek back to start
	fseek(file, 0, SEEK_SET);

	//allocate a buffer
	buffer = (u8*)linearAlloc(size);

	//read contents !
	off_t bytesRead = fread(buffer, 1, size, file);
	//u8 test = &buffer;

	//close the file because we like being nice and tidy
	fclose(file);

	csndPlaySound(8, SOUND_FORMAT_16BIT | SOUND_REPEAT, 48000, 1, 0, buffer, buffer, size);
	linearFree(buffer);
}
void audio_stop(void) {
	csndExecCmds(true);
	CSND_SetPlayState(0x8, 0);
	memset(buffer, 0, size);
	GSPGPU_FlushDataCache(buffer, size);
	linearFree(buffer);
}