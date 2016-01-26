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
#include <sfil.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
//test

#define CHUNK_NUM  15
#define ENTITY_LIST_SIZE  100
#define TERRAIN_LIST_SIZE  100
#define TEX_TABLE_SIZE  30




using namespace std;

static u8* buffer;
static u32 audioSize;

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

string get_string(int number) {
	stringstream ss;
	ss << number;
	return ss.str();
}

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

class entity {
public:
	sf2d_texture* spriteData;
	string spriteName = "NULL";
	bool visible = 1;
	bool solid = 1;
	int posX = -1;
	int posY = -1;
	int posZ = -1;

	void update() {}
};

struct textureName {
	sf2d_texture* texture;
	string name = "free";
};

struct terrain {
	sf2d_texture* spriteData;
	string textureFile;
	bool visible = true;
	bool solid = true;
};
struct point3D {
	int x = -1;
	int y = -1;
	int z = -1;
};


class gameMap {
	//private:
public:
	int ****terrainMap = new int***[15];

	//int terrainMap = new int[CHUNK_NUM][100][100][100]();
	entity entityList[ENTITY_LIST_SIZE]; //Processats individualment cada frame // HAURIA D USAR UN STD::VECTOR PER PODERLOS REORDENAR
	terrain terrainList[TERRAIN_LIST_SIZE]; //Llista de tipus de terrenys diferents, aqui és on es mirarà a partir del terrainMap
	point3D mapIndex[CHUNK_NUM]; //indica quin bloc de terreny hi ha a cada posició		
	textureName texTable[TEX_TABLE_SIZE];
	string saveName = "default";
	int freeChunkPos() { //returns the position inside texTable[] of the first free texture space
		for (int i = 0; i < CHUNK_NUM; i++) {
			if (mapIndex[i].x == -1) {
				return i;
			}
		}
		cout << "NO FREE SPACE IN MAPINDEX" << endl;
		return -1;
	}
	bool isChunkLoaded(int posX, int posY, int posZ) { // tells if said chunk is loaded in mapIndex[]
		for (int i = 0; i < CHUNK_NUM; i++) {
			if (mapIndex[i].z == posZ) {
				if (mapIndex[i].x == posX) {
					if (mapIndex[i].y == posY) {
						return 1;
					}
				}
			}
		}
		return 0;
	}
	void loadChunk(int chunkX, int chunkY, int chunkZ) { //Loads a certain chunk inside mapIndex[] and terrainMap[][][][];
		int chunkPos = freeChunkPos();
		ifstream chunkFile;
		string terrainName = ("/saves/" + saveName + "/chunks/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		chunkFile.open(terrainName);
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				for (int k = 0; k < 100; k++) {
					chunkFile >> terrainMap[chunkPos][k][j][i];
				}
			}
		}
		int emptyChunkPos = -1;
		int i = 0;
		while (emptyChunkPos == -1 && i < ENTITY_LIST_SIZE) {
			if (entityList[i].posX == -1) {
				emptyChunkPos = i;
			}
			i++;
		}
		chunkFile.close();
		ifstream entitiesFile;
		string entitiesName = ("/save/" + saveName + "/entities." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		entitiesFile.open(entitiesName);
		if (entitiesFile.is_open()) {
			while (!entitiesFile.eof()) {
				entitiesFile >> entityList[emptyChunkPos].spriteName >> entityList[emptyChunkPos].visible >> entityList[emptyChunkPos].solid >> entityList[emptyChunkPos].posX >> entityList[emptyChunkPos].posY >> entityList[emptyChunkPos].posZ;

				emptyChunkPos++;
				if (emptyChunkPos >= ENTITY_LIST_SIZE) {
					cout << "No entity space available";
				}
			}
		}

		mapIndex[chunkPos].x = chunkX;
		mapIndex[chunkPos].y = chunkY;
		mapIndex[chunkPos].z = chunkZ;
	}
	void saveChunk(int chunkPos, int chunkX, int chunkY, int chunkZ, string fileName = "default") { //unloads a chunk from memory and saves it in its file
		ofstream chunkFile;
		string mapName = ("/save/" + fileName + "/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		chunkFile.open(mapName);
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				for (int k = 0; k < 100; k++) {
					chunkFile << terrainMap[chunkPos][k][j][i];
				}
			}
		}
		int emptyChunkPos = -1;
		int i = 0;
		while (emptyChunkPos == -1 && i < ENTITY_LIST_SIZE) {
			if (entityList[i].posX == -1) {
				emptyChunkPos = i;
			}
			i++;
		}
		if (emptyChunkPos == -1) {
			emptyChunkPos = ENTITY_LIST_SIZE;
		}
		chunkFile.close();
		ofstream entitiesFile;
		string entitiesName = ("/save/" + fileName + "/entities." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		entitiesFile.open(entitiesName);

		//aqui s han de buscar entities que es trobin en el chunk que descarreguem, i tot seguit swappejar la seva posicio amb la de la ultima entity de la llista abans de les buides, i tot seguit esborrarla;

		for (int i = 0; i < ENTITY_LIST_SIZE; i++) {
			if (entityList[i].posX < 0) {
				cout << "No hi ha cap entity vàlida aqui    entityList[" << i << ']' << endl;
			}
			if (floor(entityList[i].posX / 100) == chunkX) {
				if (floor(entityList[i].posY / 100) == chunkY) {
					if (floor(entityList[i].posZ / 100) == chunkZ) {
						if (entityList[i].visible) {
							entityList[i] = entityList[emptyChunkPos - 1];
							entityList[emptyChunkPos].posX = -1;
							entityList[emptyChunkPos].posY = -1;
							entityList[emptyChunkPos].posZ = -1;
							emptyChunkPos--;
						}
					}
				}
			}
		}

		mapIndex[chunkPos].x = -1;
		mapIndex[chunkPos].y = -1;
		mapIndex[chunkPos].z = -1;
	}


	bool isTextureLoaded(string textureFile) { // tells if a texture with said name is present on texTable
		for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
			if (texTable[i].name == textureFile) {
				return 1;
			}
		}
		return 0;
	}
	int freeTexturePos() { //returns the position inside texTable[] of the first free texture space
		for (int i = 0; i < TEX_TABLE_SIZE; i++) {
			if (texTable[i].name == "free") {
				return i;
			}
		}
		cout << "NO FREE SPACE IN TEXTABLE" << endl;
		return -1;
	}
	int getTexturePos(string fileName) { //returns the position inside texTable[] of the texture with said filename
		for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
			if (texTable[i].name == fileName) {
				return i;
			}
		}
		cout << "NO TEXTURE W/ FNAME " << fileName << " FOUND" << endl;
		return -1;
	}
	void loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
		int freeTextureLoc = freeTexturePos();
		fileName = "/data/sprites/" + fileName;
		texTable[freeTextureLoc].texture = sfil_load_PNG_file(fileName.c_str(), SF2D_PLACE_RAM);
		texTable[freeTextureLoc].name = fileName;
	}
	void freeTexture(string fileName) { //frees a texture from texTable[]
		int textureLocation = getTexturePos(fileName);
		int freeTexLoc = freeTexturePos();
		textureName temp = texTable[textureLocation];
		texTable[textureLocation] = texTable[freeTexLoc - 1];
		texTable[freeTexLoc - 1] = temp;
		sf2d_free_texture(texTable[freeTexLoc - 1].texture);
		texTable[freeTexLoc - 1].name = "free";
	}
	//public:
		//FALTEN:
		//funcio per actualitzar chunks carregats, i decidir quins sobreescriure //per escollir els chunks a descarregar, restarem el numero de chunkX del player del numero del chunkX actual,
																				 //en farem el valor absolut, i li sumarem el mateix procés pero fet amb chunkY i chunkZ, això ens donara una
																				 //distància aproximada del chunk analitzat al player, i per tant, podrem descarregar el chunk més lluny
	void freeAllTextures() {	 //frees all textures
		for (int i = 0; i < TEX_TABLE_SIZE; i++) {
			texTable[i].name = "free";
			sf2d_free_texture(texTable[i].texture);
		}
	}
	bool simpleCollision(int posX, int posY, int posZ, mode collisionMode = TRRN) {	//Tells if terrain at position is occupied
		int blockX = floor(posX / 100);
		int blockY = floor(posY / 100);
		int blockZ = floor(posZ / 100);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);
		//switch case pels tipus de modes
		if (terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].solid == 1) {
			return 1;
		}
	}
	int getChunkPos(int posX, int posY, int posZ) { //returns the position inside mapIndex of the aforementioned chunk;
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

	int visibleEntity(int posX, int posY, int posZ) { // returns the position inside entityList if the entered 3d position contains an entity
		for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX == -1; i++) {
			if (entityList[i].posX < 0) {
				cout << "No s'ha trobat cap entity visible a X: " << posX << " Y: " << posY << " Z: " << posZ << endl;
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
	bool isVisible(int posX, int posY, int posZ, mode mode_t = PRRT) { //
		int blockX = floor(posX / 100);
		int blockY = floor(posY / 100);
		int blockZ = floor(posZ / 100);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);

		switch (mode_t) {
		case TRRN:
			return terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].visible;
		case NTT:
			return (visibleEntity(posX, posY, posZ) != -1);
		case PRRT:
			return ((visibleEntity(posX, posY, posZ) != -1) ? terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].visible : 1);
		}
	}

	sf2d_texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT) {
		int blockX = floor(posX / 100);
		int blockY = floor(posY / 100);
		int blockZ = floor(posZ / 100);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);
		switch (mode_t) {
		case TRRN:
			return texTable[getTexturePos((terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].textureFile))].texture;
		case NTT:
			for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX != -1; i++) {

				if (entityList[visibleEntity(posX, posY, posZ)].spriteName == texTable[i].name) {
					return texTable[i].texture;
				}
			}
			cout << "Entity texture not found in position" << posX << ' ' << posY << ' ' << posZ << endl;
		case PRRT:
			if (terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].visible == 1) {
				return terrainList[terrainMap[chunkPosition][posX - blockX * 100][posY - blockY * 100][posZ - blockZ * 100]].spriteData;
			}
			cout << "es posible k aixo sigui molt lent, sabelo";
			return texTable[getTexturePos(entityList[visibleEntity(posX, posY, posZ)].spriteName)].texture;
		}
	}
	gameMap() {
		cout << "xx";
		for (int i = 0; i < 15; i++) {
			cout << i << endl;
			terrainMap[i] = new int**[100];
			for (int j = 0; j < 100; j++) {
				terrainMap[i][j] = new int*[100];
				for (int k = 0; k < 100; k++) {
					terrainMap[i][j][k] = new int[100];
					for (int l = 0; l < 100; l++) {
						terrainMap[i][j][k][l] = 0;
					}
				}
			}
		}
	}
	void emplenarWIP() {
		for (int x = 0; x != 15; x++) {
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
};

void cameraOperation(entity player, int &cameraX, int &cameraY, const int mapHeight, const int mapWidth) {
	if (player.posX < mapWidth - 10) { if (cameraX + 14 < player.posX) { cameraX++; } }
	if (player.posX > 9) { if (cameraX + 10 > player.posX) { cameraX--; } }
	if (player.posY < mapHeight - 6) { if (cameraY + 8 < player.posY) { cameraY++; } }
	if (player.posY > 5) { if (cameraY + 6 > player.posY) { cameraY--; } }
}

class gameMain {
private:
	u32 kDown;
	u32 kHeld;
	u32 kUp;
	gameMap map;
	entity player;
	void gameLoop() {

		//processar input
		//refrescar coses carregades
		//dibuixar
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		for (int i = 0; i != 15; i++) {
			for (int j = 0; j != 25; j++) {
				sf2d_draw_texture(map.getTexture(150 + i, 150 + j, 150), j * 16, i * 16);
			}
		}
		sf2d_end_frame();
		//fdibuixar
	}
public:
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
		string saveName = "default";
		ifstream general;
		general.open("saves/" + saveName + "/general.txt");
		string playerName;
		general >> playerName >> player.posX >> player.posY >> player.posZ;
		//loadmap
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				//	for (int k = 0; k < 3; k++) {
				map.loadChunk(i, j,/* k);*/ 1);
				//	}
			}
		}
		map.loadTexture("wall.png");
		map.loadTexture("player.png");
		map.loadTexture("grass.png");
		map.entityList[0].posX = 149;
		map.entityList[0].posY = 149;
		map.entityList[0].posZ = 149;
		map.entityList[0].solid = 1;
		map.entityList[0].spriteName = "player.png";
		map.entityList[0].visible = 1;

		map.terrainList[0].solid = 0;
		map.terrainList[0].visible = 0;
		map.terrainList[1].solid = 1;
		map.terrainList[1].visible = 1;
		map.terrainList[1].textureFile = "grass.png";
		while (1) {
			cout << "soc viu tete";
			gameLoop();
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
	sf2d_set_clear_color(RGBA8(0x31, 0xb0, 0x15, 0xFF));
	string saveName = "default";	   //guarro 
	gameMain gameMain1;
	gameMain1.gameCore();
	audio_stop();
	csndExit();
	// Exit
	//map.freeAllTextures();

	sf2d_fini();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();

	return 1;
}