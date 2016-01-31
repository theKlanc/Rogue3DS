/*
TO-DO
-88-generació de mapa interessant i otfly
-87-arreglar savechunk
8-afegir events
9-afegir combat
10-millorar el so i afegir sfx
11-passar a romfs

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

#define CHUNK_SIZE 100
#define CHUNK_NUM  19 //( 19 = rubik's (n=2) - corners, 11 = baldufa(n = 2, però z pondera més) , 7 = rubik's core (n = 1))  la n3ds aguanta almenys 80, la old 26
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

	csndPlaySound(8, SOUND_FORMAT_8BIT | SOUND_REPEAT, 48000, 1, 0, buffer, buffer, audioSize);
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

enum nttype {
	NPC,
	PLR,
};

enum direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FRONT,
	BACK,
};
struct point3D {
	int x = -1;
	int y = -1;
	int z = -1;
};
struct entity {
	string spriteName = "ENULL";
	bool visible = 1;
	bool solid = 1;
	int posX = -1;
	int posY = -1;
	int posZ = -1;
	nttype type = NPC;
};

struct textureName {
	sf2d_texture* texture;
	string name = "free";
};

struct terrain {
	string textureFile = "TNULL";
	bool visible = true;
	bool solid = true;
};



class gameMap {
	//private:
public:
	u8 ****terrainMap = new u8***[CHUNK_NUM];

	entity entityList[ENTITY_LIST_SIZE]; //Processats individualment cada frame // HAURIA D USAR UN STD::VECTOR PER PODERLOS REORDENAR
	terrain terrainList[TERRAIN_LIST_SIZE]; //Llista de tipus de terrenys diferents, aqui és on es mirarà a partir del terrainMap
	point3D mapIndex[CHUNK_NUM]; //indica quin bloc de terreny hi ha a cada posició		
	textureName texTable[TEX_TABLE_SIZE];
	string saveName = "default";
	int chunkValue(point3D chunkN, point3D chunkO) {
		return (abs(chunkN.x - chunkO.x) + abs(chunkN.y - chunkO.y) + abs(chunkN.z - chunkO.z));
	}
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
	void saveChunk(int chunkX, int chunkY, int chunkZ) { //unloads a chunk from memory and saves it in its file
		int chunkPos = getChunkPos(chunkX, chunkY, chunkZ);
		ofstream chunkFile;
		string mapName = ("saves/" + saveName + "/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		chunkFile.open(mapName);
		if (!chunkFile.is_open()) {
			cout << "couldn't open file: " << mapName << endl;
		}
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
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
		string entitiesName = ("saves/" + saveName + "/entities." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		entitiesFile.open(entitiesName);
		if (!entitiesFile.is_open()) {
			cout << "couldn't open file: " << entitiesName << endl;
		}

		//aqui s han de buscar entities que es trobin en el chunk que descarreguem, i tot seguit swappejar la seva posicio amb la de la ultima entity de la llista abans de les buides, i tot seguit esborrarla;

		for (int i = 0; i < ENTITY_LIST_SIZE; i++) {
			if (entityList[i].posX < 0) {
				cout << "No hi ha cap entity vàlida aqui    entityList[" << i << ']' << endl;
			}
			if (floor(entityList[i].posX / CHUNK_SIZE) == chunkX) {
				if (floor(entityList[i].posY / CHUNK_SIZE) == chunkY) {
					if (floor(entityList[i].posZ / CHUNK_SIZE) == chunkZ) {
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

	void freeAChunk(point3D playerPos) {
		point3D playerChunk;
		playerChunk.x = playerPos.x / CHUNK_SIZE;
		playerChunk.y = playerPos.y / CHUNK_SIZE;
		playerChunk.z = playerPos.z / CHUNK_SIZE;
		for (int i = 0; i < CHUNK_NUM; i++) {
			point3D chunkN = mapIndex[i];
			if (chunkValue(chunkN, playerChunk) > 2) {
				saveChunk(chunkN.x, chunkN.y, chunkN.z);
				return;
			}
		}
	}
	void loadChunk(int chunkX, int chunkY, int chunkZ, point3D playerPos) { //Loads a certain chunk inside mapIndex[] and terrainMap[][][][];
		int chunkPos = freeChunkPos();
		if (chunkPos == -1) {
			freeAChunk(playerPos);
		}
		ifstream chunkFile;
		string terrainName = ("saves/" + saveName + "/chunks/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		chunkFile.open(terrainName);
		if (!chunkFile.is_open()) {
			cout << "couldn't open file: " << terrainName << endl;
		}
		int currentTerrain;
		int amountTerrain;
		chunkFile >> currentTerrain >> amountTerrain;
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if (amountTerrain == 0) {
						chunkFile >> currentTerrain >> amountTerrain;
					}
					terrainMap[chunkPos][k][j][i] = currentTerrain;
					amountTerrain--;
				}
			}
		}
		/*int emptyChunkPos = -1;
		int i = 0;
		while (emptyChunkPos == -1 && i < ENTITY_LIST_SIZE) {
			if (entityList[i].posX == -1) {
				emptyChunkPos = i;
			}
			i++;
		}*/
		int emptyChunkPos = freeChunkPos();
		cout << "freechunkPos: " << emptyChunkPos << endl;
		chunkFile.close();
		ifstream entitiesFile;
		string entitiesName = ("saves/" + saveName + "/entities." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		entitiesFile.open(entitiesName);
		if (!entitiesFile.is_open()) {
			cout << "couldn't open file: " << entitiesName << endl;
		}
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
	void loadTerrainTable() {
		ifstream terrainTable;
		string terrainName = ("saves/" + saveName + "/terrainTable.txt");
		terrainTable.open(terrainName);
		if (!terrainTable.is_open()) {
			cout << "error opening terrainTable" << endl;

			return;
		}
		int i = 0;
		while (!terrainTable.eof()) {
			terrainTable >> terrainList[i].textureFile >> terrainList[i].visible >> terrainList[i].solid;
			if (terrainList[i].visible) {
				loadTexture(terrainList[i].textureFile);
			}
			i++;
		}
		terrainTable.close();
	}
	void loadNewChunk(point3D playerPos) {
		point3D playerChunk;
		playerChunk.x = playerPos.x / CHUNK_SIZE;
		playerChunk.y = playerPos.y / CHUNK_SIZE;
		playerChunk.z = playerPos.z / CHUNK_SIZE;
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				for (int k = -1; k < 2; k++)
				{
					point3D chunkPos;
					chunkPos.x = playerChunk.x + i;
					chunkPos.y = playerChunk.y + j;
					chunkPos.z = playerChunk.z + k;
					cout << chunkValue(chunkPos, playerChunk) << endl;
					if (chunkValue(chunkPos, playerChunk) <= 2) {
						if (isChunkLoaded(chunkPos.x, chunkPos.y, chunkPos.z) == 0) {
							loadChunk(chunkPos.x, chunkPos.y, chunkPos.z, playerPos);
							return;
						}
					}
				}
			}
		}
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
		texTable[freeTextureLoc].name = fileName;
		fileName = "data/sprites/" + fileName;
		texTable[freeTextureLoc].texture = sfil_load_PNG_file(fileName.c_str(), SF2D_PLACE_RAM);
		sf2d_texture_tile32(texTable[freeTextureLoc].texture);

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
		int blockX = floor(posX / CHUNK_SIZE);
		int blockY = floor(posY / CHUNK_SIZE);
		int blockZ = floor(posZ / CHUNK_SIZE);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);
		if (chunkPosition == -1) {
			return 0;
		}
		//switch case pels tipus de modes
		if (terrainList[terrainMap[chunkPosition][posX - (blockX * CHUNK_SIZE)][posY -( blockY * CHUNK_SIZE)][posZ - (blockZ * CHUNK_SIZE)]].solid == 1) {
			return 1;
		}
		return 0;
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
		return -1;
	}

	int visibleEntity(int posX, int posY, int posZ) { // returns the position inside entityList if the entered 3d position contains a visible entity
		for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX != -1; i++) {
			if (entityList[i].visible) {
				if (entityList[i].posX == posX) {
					if (entityList[i].posY == posY) {
						if (entityList[i].posZ == posZ) {
							return i;
						}
					}
				}
			}
		}
		return -1;
	}
	bool isVisible(int posX, int posY, int posZ, mode mode_t = PRRT) { //
		int blockX = floor(posX / CHUNK_SIZE);
		int blockY = floor(posY / CHUNK_SIZE);
		int blockZ = floor(posZ / CHUNK_SIZE);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);
		if (chunkPosition == -1) {
			return (visibleEntity(posX, posY, posZ) != -1);
		}

		switch (mode_t) {
		case TRRN:
			return terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].visible;
		case NTT:
			return (visibleEntity(posX, posY, posZ) != -1);
		case PRRT:
			return (isVisible(posX, posY, posZ, TRRN)) ? 1 : isVisible(posX, posY, posZ, NTT);
		}
	}

	sf2d_texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT) {
		int blockX = floor(posX / CHUNK_SIZE);
		int blockY = floor(posY / CHUNK_SIZE);
		int blockZ = floor(posZ / CHUNK_SIZE);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);
		if (chunkPosition == -1) {
			for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX != -1; i++) {

				if (entityList[visibleEntity(posX, posY, posZ)].spriteName == texTable[i].name) {
					return texTable[i].texture;
				}
			}
		}
		switch (mode_t) {

		case TRRN:
			return texTable[getTexturePos(terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].textureFile)].texture;
		case NTT:
			for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX != -1; i++) {

				if (entityList[visibleEntity(posX, posY, posZ)].spriteName == texTable[i].name) {
					return texTable[i].texture;
				}
			}
			cout << "Entity texture not found in position" << posX << ' ' << posY << ' ' << posZ << endl;
		case PRRT:
			if (isVisible(posX, posY, posZ, NTT) == 1) {
				return texTable[getTexturePos(entityList[visibleEntity(posX, posY, posZ)].spriteName)].texture;
			}

			else if (isVisible(posX, posY, posZ, TRRN)) {
				return texTable[getTexturePos(terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].textureFile)].texture;
			}
		}
	}
	gameMap() {
		for (int i = 0; i < CHUNK_NUM; i++) {
			terrainMap[i] = new u8**[CHUNK_SIZE];
			for (int j = 0; j < CHUNK_SIZE; j++) {
				terrainMap[i][j] = new u8*[CHUNK_SIZE];
				for (int k = 0; k < CHUNK_SIZE; k++) {
					terrainMap[i][j][k] = new u8[CHUNK_SIZE];
					for (int l = 0; l < CHUNK_SIZE; l++) {
						terrainMap[i][j][k][l] = 1;
					}
				}
			}
		}
	}
	void emplenarWIP() {
		for (int x = 0; x != CHUNK_NUM; x++) {
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
	entity* player;
	bool o = 0;
	sf2d_texture *overlay;
	u64 loop = 0;
	
	void exitGame() {
		audio_stop();
		csndExit();
		sf2d_fini();
		gfxExit();
		hidExit();
		aptExit();
		srvExit();
		exit(1);
	}
	void moveEntity(entity &currentEntity, direction dir, bool autojump = true) {
		switch (dir) {
		case DOWN:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ - 1) == 0) {
				currentEntity.posZ--;
			}
			break;
		case UP:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0) {
				currentEntity.posZ++;
			}
			break;
		case FRONT:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY - 1, currentEntity.posZ) == 0) {
				currentEntity.posY--;
			}
			else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX, currentEntity.posY - 1, currentEntity.posZ + 1) == 0) {
				currentEntity.posY--;
				currentEntity.posZ++;
			}
			break;
		case BACK:
			if (map.simpleCollision(currentEntity.posX, currentEntity.posY + 1, currentEntity.posZ) == 0) {
				currentEntity.posY++;
			}
			else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX, currentEntity.posY + 1, currentEntity.posZ + 1) == 0) {
				currentEntity.posY++;
				currentEntity.posZ++;
			}
			break;
		case LEFT:
			if (map.simpleCollision(currentEntity.posX - 1, currentEntity.posY, currentEntity.posZ) == 0) {
				currentEntity.posX--;
			}
			else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX - 1, currentEntity.posY, currentEntity.posZ + 1) == 0) {
				currentEntity.posX--;
				currentEntity.posZ++;
			}
			break;
		case RIGHT:
			if (map.simpleCollision(currentEntity.posX + 1, currentEntity.posY - 1, currentEntity.posZ) == 0) {
				currentEntity.posX++;
			}
			else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX + 1, currentEntity.posY, currentEntity.posZ + 1) == 0) {
				currentEntity.posX++;
				currentEntity.posZ++;
			}
			break;
		}
	}

	void updateEntity(entity &currentEntity) {	
		cout << "updating entities " << endl;
		if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ - 1) == 0) {
			cout << "que es faci la gravetat " << endl;
			currentEntity.posZ--;
			if (currentEntity.posZ < 0) {
				cout << "has caigut del mon, capoll" << endl;
				svcSleepThread(3000000000);
				exitGame();
			}
		}
		switch (currentEntity.type) {
		case NPC:
			break;
		}
	}
	void updateEntities() {
		for (int i = 0; i < ENTITY_LIST_SIZE && map.entityList[i].posX >= 0; i++)
		{
			updateEntity(map.entityList[i]);
		}
	}

	void drawFrame() {
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		for (int i = 0; i != 15; i++) {
			for (int j = 0; j != 25; j++) {
				if (map.isVisible((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - 1))) {
					sf2d_draw_texture(map.getTexture((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - 1)), j * 16, i * 16);
				}
				if (map.isVisible((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ))) {
					sf2d_draw_texture(map.getTexture((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ)), j * 16, i * 16);
				}
			}
		}
		if (o) {
			sf2d_draw_texture(overlay, 0, 0);
		}
		sf2d_end_frame();
	}
	void handleInput() {
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		if (kDown & KEY_START) {
			exitGame();
		}
		if (kHeld & KEY_SELECT) {
			o = 1;
		}
		else o = 0;
		if (kHeld & KEY_RIGHT) {
			moveEntity(*player, RIGHT);
		}
		if (kHeld & KEY_LEFT) {
			moveEntity(*player, LEFT);
		}
		if (kHeld & KEY_UP) {
			moveEntity(*player, FRONT);
		}
		if (kHeld & KEY_DOWN) {
			moveEntity(*player, BACK);
		}
		if (kHeld & KEY_A) {
			moveEntity(*player, UP);
		}

	}
	void gameLoop() {

		updateEntities();
		handleInput();
		cout << player->posX << ' ' << player->posY << ' ' << player->posZ << endl;
		sf2d_swapbuffers();
		//processar input
		//refrescar coses carregades
		//dibuixar
		drawFrame();
		//FALTA intentar carregar chunks a cada loop, i crearlos on the fly, a més s haurien de fer bé, i ficarlos en un thread separat al carregar

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
		
		overlay = sfil_load_PNG_file("data/sprites/overlay.png", SF2D_PLACE_RAM);
		player = &map.entityList[0];
		string saveName = "default";
		ifstream general;
		general.open("saves/" + saveName + "/general.txt");
		if (!general.is_open()) {
			cout << "couldn't open file: " << ("saves/" + saveName + "/general.txt") << endl;
		}
		string playerSprite, playerName;
		general >> playerName >> player->spriteName >> player->posX >> player->posY >> player->posZ;
		cout << playerName << endl;
		point3D playerPos;
		cout << player->posX << endl;
		playerPos.x = player->posX;
		playerPos.y = player->posY;
		playerPos.z = player->posZ;
		//loadmap
		for (int i = 0; i < CHUNK_NUM; i++) {
			map.loadNewChunk(playerPos);
		}

		map.loadTexture("player.png");
		map.loadTerrainTable();
		cout << "vram: " << vramSpaceFree() << endl;
		cout << "mappable: " << mappableSpaceFree() << endl;
		cout << "linear: " << linearSpaceFree() << endl;
		//audio_load("data/sounds/bgm/wilderness.raw"); //[N3DS] only 
		while (1) {
			if (loop % 60 == 0) { gameLoop(); }
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