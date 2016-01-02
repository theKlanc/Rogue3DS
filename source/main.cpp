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
#include <vector>
//test

#define CHUNK_NUM  27
#define ENTITY_LIST_SIZE  100
#define TERRAIN_LIST_SIZE  100



using namespace std;

enum mode {
	PRRT,
	TRRN,
	NTT,
	RAY,
	BLCK,
};

enum direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FRONT,
	BACK,
};

extern "C" {
	extern const struct {
		unsigned int 	 width;
		unsigned int 	 height;
		unsigned int 	 bytes_per_pixel;
		unsigned char	 pixel_data[];
	} dwarf_img, wall_img, field_img;
}

class entity { // es podrien passar a classe
	public:
		sf2d_texture* spriteData;
		bool visible = true;
		int posX;
		int posY;
		int posZ;
		bool solid;

		void update(){}
};

struct terrain {
	sf2d_texture* spriteData;
	bool visible = false;
	bool solid;
};
struct point3D {
	int x;
	int y;
	int z;
};


class mapa {
	//TO-DO
	//Funcio per carregar sprite en memoria
	//Funcio per carregar chunks a memoria
	//
private:
	static int terrainMap[CHUNK_NUM][100][100][100];
	entity entityList[ENTITY_LIST_SIZE]; //Processats individualment cada frame // HAURIA D USAR UN STD::VECTOR PER PODERLOS REORDENAR
	terrain terrainList[TERRAIN_LIST_SIZE]; //Llista de tipus de terrenys diferents, aqui és on es mirarà a partir del terrainMap
	point3D mapIndex[CHUNK_NUM]; //indica quin bloc de terreny hi ha a cada posició		
	sf2d_texture* texTable[30];
public:
	//FALTEN:
	//funcio per carregar chunks al terrainMap adequat, donats un cert arxiu i la posicio a on inserirlo
	//juntament amb aquesta, funcio per carregar els entities corresponents a akest chunk
	//funcio per guardar chunks al terrainMap adequat, donats un cert arxiu i la posicio a on inserirlo
	//juntament amb aquesta, funcio per guardar els entities corresponents a akest chunk
	//funcio per actualitzar chunks carregats, i decidir quins sobreescriure //per escollir els chunks a descarregar, restarem el numero de chunkX del player del numero del chunkX actual,
																			 //en farem el valor absolut, i li sumarem el mateix procés pero fet amb chunkY i chunkZ, això ens donara una
																			 //distància aproximada del chunk analitzat al player, i per tant, podrem descarregar el chunk més llunyà
	//fisiques, amb modes ray i block


	int getChunk(int posX, int posY, int posZ) {
		for (int i = 0; i < CHUNK_NUM; i++) {
			if (posX == mapIndex[i].x) {
				if (posY == mapIndex[i].y) {
					if (posZ == mapIndex[i].z) {
						return i;
					}
				}
			}
		}
		cout << "No s'ha trobat cap chunk carregat amb aquestes coordenades" << endl << "X:" << posX << endl << "Y:" << posY << endl << "Z:" << posZ << endl;
		return -1;
	}

	int visibleEntity(int posX, int posY, int posZ) { 
		while (1) {
			for (int i = 0; i < ENTITY_LIST_SIZE; i++) {
				if (entityList[i].posX < 0) {
					cout << "No s'ha trobat cap entity visible en aquesta posicio" << endl;
					return -1;
				}
				if (entityList[i].posX == posX) {
					if (entityList[i].posY == posY) {
						if (entityList[i].posZ == posZ) {
							if (entityList[i].visible) {
								return i;
							}

						}
					}
				}
			}
		}
	}
	bool isVisible(int posX, int posY, int posZ, mode mode_t = PRRT) {
		int blockX = floor(posX / 100);
		int blockY = floor(posY / 100);
		int blockZ = floor(posZ / 100);
		int chunkPosition = getChunk(blockX, blockY, blockZ);

		switch (mode_t) {
		case TRRN:
			return terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].visible;
		case NTT:
			return (visibleEntity(posX, posY, posZ)==-1);
		case PRRT:
			return ((visibleEntity(posX, posY, posZ) != -1) ? terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].visible : 1);
		}
	}

	sf2d_texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT) {
		int blockX = floor(posX / 100);
		int blockY = floor(posY / 100);
		int blockZ = floor(posZ / 100);
		int chunkPosition = getChunk(blockX, blockY, blockZ);
		switch (mode_t) {
		case TRRN:
			return terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].spriteData;
		case NTT:
			return entityList[visibleEntity(posX,posY,posZ)].spriteData;
		case PRRT:
			return ((visibleEntity(posX,posY,posZ)!=-1) ? entityList[visibleEntity(posX, posY, posZ)].spriteData : terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].spriteData);
		}
	}
	map() {
		//inicialitzar la entityList amb tot a -1
		//quan esborrem algo de la entitylist, primer el swappejem per el entity en ultima posició, i després substituim tots els seus paramtres per -1;
		//
		terrainList[0].solid = 0;
		terrainList[1].solid = 1;
		texTable[0] = sf2d_create_texture(dwarf_img.width, dwarf_img.height, TEXFMT_RGBA8, SF2D_PLACE_RAM);   //s ha de fer bbbbbbb
		sf2d_fill_texture_from_RGBA8(texTable[0], dwarf_img.pixel_data, dwarf_img.width, dwarf_img.height);
		sf2d_texture_tile32(texTable[0]);
	}
	void emplenarWIP() {
		for (int x = 0; x != 27; x++) {
			for (int i = 0; i <= 99; i++) {
				for (int j = 0; i <= 99; j++) {
					for (int k = 0; k <= 1; k++) {
						terrainMap[x][i][j][49 + k] = 1;
					}
				}
			}
			for (int i = 1; i != 99; i++) {
				for (int j = 1; i != 99; j++) {
					terrainMap[x][i][j][50] = 0;
				}
			}
		}

	}
	//void loadChunk(int chunkX,int chunkY,int chunkZ, int fillX, int fillY, int fillZ)
	/*bool collision(int x, int y, int z) {
	//ultra-wip falta abstracció pel mapa
	}*/
};

void cameraOperation(entity player, int &cameraX, int &cameraY, const int mapHeight, const int mapWidth) {
	if (player.posX < mapWidth - 10) { if (cameraX + 14 < player.posX) { cameraX++; } }
	if (player.posX > 9) { if (cameraX + 10 > player.posX) { cameraX--; } }
	if (player.posY < mapHeight - 6) { if (cameraY + 8 < player.posY) { cameraY++; } }
	if (player.posY > 5) { if (cameraY + 6 > player.posY) { cameraY--; } }
}


static u8* buffer;
static u32 audioSize;

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
	char temp = 0;
	bool stop = 0;
	const short mapHeight = 2000;
	const short mapWidth = 2000;
	static short mapa[mapWidth][mapHeight]; //necessita memalloc
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
			mapa[j][i] = 2;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++) {
		for (int j = 1; j != mapWidth - 1; j++) {
			mapa[j][i] = 1;
		}
	}
	for (int i = 1; i != mapHeight - 1; i++) {
		for (int j = 1; j != mapWidth - 1; j++) {
			if (rand() % 20 == 0) { mapa[j][i] = 2; }
		}
	}

	player.posX = 5;
	player.posY = 5;
	mapa[5][5] = 9;

	csndInit();//start Audio Lib
	audio_load("audio/original_raw.bin");
	static map mapTest;
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
				if (!list[mapa[player.posX][player.posY - 1]]->solid) {
					mapa[player.posX][player.posY] = 1;
					player.posY--;
					mapa[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_LEFT) {
				if (!list[mapa[player.posX - 1][player.posY]]->solid) {
					mapa[player.posX][player.posY] = 1;
					player.posX--;
					mapa[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_RIGHT) {
				if (!list[mapa[player.posX + 1][player.posY]]->solid) {
					mapa[player.posX][player.posY] = 1;
					player.posX++;
					mapa[player.posX][player.posY] = 9;
				}
			}
			if (kHeld & KEY_DOWN) {
				if (!list[mapa[player.posX][player.posY + 1]]->solid) {
					mapa[player.posX][player.posY] = 1;
					player.posY++;
					mapa[player.posX][player.posY] = 9;

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
				sf2d_draw_texture(list[mapa[j + cameraX][i + cameraY]]->spriteData, j * 16, i * 16);
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
	off_t audioSize = ftell(file);

	// seek back to start
	fseek(file, 0, SEEK_SET);

	//allocate a buffer
	buffer = (u8*)linearAlloc(audioSize);

	//read contents !
	off_t bytesRead = fread(buffer, 1, audioSize, file);
	//u8 test = &buffer;

	//close the file because we like being nice and tidy
	fclose(file);

	csndPlaySound(8, SOUND_FORMAT_16BIT | SOUND_REPEAT, 48000, 1, 0, buffer, buffer, audioSize);
	linearFree(buffer);
}
void audio_stop(void) {
	csndExecCmds(true);
	CSND_SetPlayState(0x8, 0);
	memset(buffer, 0, audioSize);
	GSPGPU_FlushDataCache(buffer, audioSize);
	linearFree(buffer);
}