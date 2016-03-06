#ifdef _WIN32
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long
#define s8 char
#define s16 short
#define s32 int
#define s64 long
#else
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>

#define CHUNK_SIZE 100
#define CHUNK_NUM  19 //( 19 = rubik's sense corners (n=2) - corners, 11 = baldufa(n = 2, però z pondera més) , 7 = rubik's core (n = 1))  la n3ds aguanta almenys 80, la old 26
#define ENTITY_LIST_SIZE  100
#define TERRAIN_LIST_SIZE  100
#define TEX_TABLE_SIZE  30
#define RENDER_HEIGHT 1
#define FLOOR_HEIGHT 550



using namespace std;

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
#ifdef _WIN32
	sf::Texture texture;
#else
	sf2d_texture* texture;
#endif
	string name = "free";
};

struct terrain {
	string textureFile = "TNULL";
	bool visible = true;
	bool solid = true;
};



void saveMap(u8*** map, int x, int y, int z, string saveName) {
	//cout<< "ara el guardo" << endl;
	clock_t t = clock();
	ofstream chunkFile;
	string filename = "saves/" + saveName + "/chunks/terrain.";
	filename = (filename + get_string(x) + '.' + get_string(y) + '.' + get_string(z));
	chunkFile.open(filename, ios_base::binary);
	char current = 255;
	int num = 0;
	stringstream sstream;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				if ((int)map[k][j][i] != current) {
					sstream << (int)current << ' ' << num << ' ';
					current = (int)map[k][j][i];
					num = 1;
				}
				else {
					num++;
					map[k][j][i] = 1;
				}
			}
		}
	}
	sstream << (int)current << ' ' << num << ' ';
	//cout<< "               ara el vomito" << endl;
	chunkFile << sstream.rdbuf();
	//cout<< "               done" << endl;
	chunkFile.flush();
	chunkFile.close();
	//cout<< "                 CSAVE " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 * 10000 << endl;
}

void createMap(string saveName, int chunkX, int chunkY, int chunkZ) {
	//cout<< "intento crear un chunk a " << chunkX << chunkY << chunkZ << endl;
	clock_t t = clock();
	u8 ***newChunk = new u8**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++) {
		newChunk[i] = new u8*[CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++) {
			newChunk[i][j] = new u8[CHUNK_SIZE];
			for (int k = 0; k < CHUNK_SIZE; k++) {
				newChunk[i][j][k] = 1;
			}
		}
	}

	ofstream file;
	string filename = "saves/" + saveName + "/chunks/terrain.";
	stringstream sstream;
	filename = (filename + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
	file.open(filename, ios_base::binary);
	if (chunkZ < FLOOR_HEIGHT / CHUNK_SIZE) {
		sstream << 1 << ' ' << pow(CHUNK_SIZE, 3) << ' ';
		file << sstream.rdbuf();
		file.close();
		//cout<< "                 CREATE1 " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
	}
	else if (chunkZ == FLOOR_HEIGHT / CHUNK_SIZE) {
		for (int n = 0; n < CHUNK_SIZE; n++) {
			for (int m = 0; m < CHUNK_SIZE; m++) {
				for (int l = 0; l < CHUNK_SIZE; l++) {
					if (n < CHUNK_SIZE / 2) {
						newChunk[l][m][n] = 1;
					}
					else if (n == CHUNK_SIZE / 2) {
						int bloc = rand() % 7;
						if (bloc == 1) { bloc++; }
						else bloc = 0;
						newChunk[l][m][n] = bloc;
					}
					else {
						newChunk[l][m][n] = 0;
					}
				}
			}
		}
		//cout<< "                 CREATE2 " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
		saveMap(newChunk, chunkX, chunkY, chunkZ, saveName);
	}
	else {
		file << 0 << ' ' << pow(CHUNK_SIZE, 3) << ' ';
		file << sstream.rdbuf();
		file.flush();
		file.close();
		//cout<< "                 CREATE3 " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
	}

}



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
		int res = (pow(chunkN.x - chunkO.x, 2) + pow(chunkN.y - chunkO.y, 2) + pow(chunkN.z - chunkO.z, 2));
		return res;
	}
	int freeChunkPos() { //returns the position inside texTable[] of the first free texture space
		for (int i = 0; i < CHUNK_NUM; i++) {
			if (mapIndex[i].x == -1) {
				return i;
			}
		}
		//cout<< "NO FREE SPACE IN MAPINDEX" << endl;
		return -1;
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
														 //cout<< "saving chunk " << chunkX << chunkY << chunkZ << endl;
		clock_t t = clock();
		int chunkPos = getChunkPos(chunkX, chunkY, chunkZ);
		if (chunkPos == -1) {
			//cout<< "chunk is already unloaded";
			return;
		}
		ofstream chunkFile;
		string terrainName = ("saves/" + saveName + "/chunks/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		chunkFile.open(terrainName, ios_base::binary);
		if (!chunkFile.is_open()) {
			//cout<< "couldn't open file: " << terrainName << endl;
		}
		char current = 255;
		int num = 0;
		stringstream sstream;
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if ((int)terrainMap[chunkPos][k][j][i] != current) {
						sstream << (int)current << ' ' << num << ' ';
						current = (int)terrainMap[chunkPos][k][j][i];
						num = 1;
					}
					else {
						num++;
						terrainMap[chunkPos][k][j][i] = 1;
					}
				}
			}
		}
		sstream << (int)current << ' ' << num << ' ';
		chunkFile << sstream.rdbuf();
		chunkFile.flush();
		chunkFile.close();

		mapIndex[chunkPos].x = -1;
		mapIndex[chunkPos].y = -1;
		mapIndex[chunkPos].z = -1;
		//cout<< "                 SAVE " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
		//cout<< "done saving" << endl;
	}

	void freeAChunk(point3D playerPos) {
		clock_t t = clock();
		point3D playerChunk;
		playerChunk.x = playerPos.x / CHUNK_SIZE;
		playerChunk.y = playerPos.y / CHUNK_SIZE;
		playerChunk.z = playerPos.z / CHUNK_SIZE;
		for (int i = 0; i < CHUNK_NUM; i++) {
			point3D chunkN = mapIndex[i];
			if (chunkValue(chunkN, playerChunk) > 2) {
				saveChunk(chunkN.x, chunkN.y, chunkN.z);
				//cout<< "                 FREE " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
				return;
			}
		}//[DEBUG]
		 //cout<< "tiu, no se que passa, pero algo va mal";
		for (int i = 0; i < CHUNK_NUM; i++) {
			////cout<< chunkValue(mapIndex[i], playerPos) << endl;
		}
		//[/DEBUG]

	}
	void loadChunk(int chunkX, int chunkY, int chunkZ, point3D playerPos) { //Loads a certain chunk inside mapIndex[] and terrainMap[][][][];
		clock_t t = clock();
		int chunkPos = freeChunkPos();
		if (chunkPos == -1) {
			freeAChunk(playerPos);
			chunkPos = freeChunkPos();
		}
		ifstream chunkFile;
		string terrainName = ("saves/" + saveName + "/chunks/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		chunkFile.open(terrainName, ios_base::binary);
		if (!chunkFile.is_open()) {
			//cout<< "couldn't open file: " << terrainName << endl;
			createMap(saveName, chunkX, chunkY, chunkZ);
			chunkFile.open(terrainName, ios_base::binary);
		}
		int currentTerrain;
		int amountTerrain;
		stringstream sstream;
		sstream << chunkFile.rdbuf();
		sstream >> currentTerrain >> amountTerrain;
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					if (amountTerrain == 0) {
						sstream >> currentTerrain >> amountTerrain;
					}
					terrainMap[chunkPos][k][j][i] = currentTerrain;
					amountTerrain--;
				}
			}
		}
		int emptyChunkPos = freeChunkPos();
		chunkFile.close();
		ifstream entitiesFile;
		string entitiesName = ("saves/" + saveName + "/entities." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
		entitiesFile.open(entitiesName);
		if (!entitiesFile.is_open()) {
			//cout<< "couldn't open file: " << entitiesName << endl;
		}
		if (entitiesFile.is_open()) {
			while (!entitiesFile.eof()) {
				entitiesFile >> entityList[emptyChunkPos].spriteName >> entityList[emptyChunkPos].visible >> entityList[emptyChunkPos].solid >> entityList[emptyChunkPos].posX >> entityList[emptyChunkPos].posY >> entityList[emptyChunkPos].posZ;

				emptyChunkPos++;
				if (emptyChunkPos >= ENTITY_LIST_SIZE) {
					//cout<< "No entity space available";
				}
			}
		}

		mapIndex[chunkPos].x = chunkX;
		mapIndex[chunkPos].y = chunkY;
		mapIndex[chunkPos].z = chunkZ;
		//cout<< "                 LOAD " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
	}
	void loadTerrainTable() {
		ifstream terrainTable;
		string terrainName = ("saves/" + saveName + "/terrainTable.txt");
		terrainTable.open(terrainName);
		if (!terrainTable.is_open()) {
			//cout<< "error opening terrainTable" << endl;

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
		//cout<< "loudNewChunk" << endl;
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
					if (chunkPos.x >= 0 && chunkPos.y >= 0 && chunkPos.z >= 0)
						if (chunkValue(chunkPos, playerChunk) <= 2) {
							if (isChunkLoaded(chunkPos.x, chunkPos.y, chunkPos.z) == 0) {
								//cout<< "trying to load chunk " << chunkPos.x << chunkPos.y << chunkPos.z << endl;
								if (freeChunkPos() == -1) {
									freeAChunk(playerPos);
									if (freeChunkPos() == -1) {
										return;
									}
								}
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
		//cout<< "NO FREE SPACE IN TEXTABLE" << endl;
		return -1;
	}
	int getTexturePos(string fileName) { //returns the position inside texTable[] of the texture with said filename
		for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
			if (texTable[i].name == fileName) {
				return i;
			}
		}
		//cout<< "NO TEXTURE W/ FNAME " << fileName << " FOUND" << endl;
		return -1;
	}
	void loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
		int freeTextureLoc = freeTexturePos();
		texTable[freeTextureLoc].name = fileName;
		fileName = "data/sprites/" + fileName;
#ifdef _WIN32
		texTable[freeTextureLoc].texture.loadFromFile(fileName);
		texTable[freeTextureLoc].texture.setSmooth(false);
#else
		texTable[freeTextureLoc].texture = sfil_load_PNG_file(fileName.c_str(), SF2D_PLACE_RAM);
		sf2d_texture_tile32(texTable[freeTextureLoc].texture);
#endif

	}
	void freeTexture(string fileName) { //frees a texture from texTable[]
		int textureLocation = getTexturePos(fileName);
		int freeTexLoc = freeTexturePos();
		textureName temp = texTable[textureLocation];
		texTable[textureLocation] = texTable[freeTexLoc - 1];
		texTable[freeTexLoc - 1] = temp;
		texTable[freeTexLoc - 1].name = "free";
#ifndef _WIN32
		sf2d_free_texture(texTable[freeTexLoc - 1].texture);
#endif
	}
	//public:
	//FALTEN:
	//funcio per actualitzar chunks carregats, i decidir quins sobreescriure //per escollir els chunks a descarregar, restarem el numero de chunkX del player del numero del chunkX actual,
	//en farem el valor absolut, i li sumarem el mateix procés pero fet amb chunkY i chunkZ, això ens donara una
	//distància aproximada del chunk analitzat al player, i per tant, podrem descarregar el chunk més lluny
	void freeAllTextures() {	 //frees all textures
		for (int i = 0; i < TEX_TABLE_SIZE; i++) {
			texTable[i].name = "free";
#ifndef _WIN32
			sf2d_free_texture(texTable[i].texture);
#endif
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
		if (terrainList[terrainMap[chunkPosition][posX - (blockX * CHUNK_SIZE)][posY - (blockY * CHUNK_SIZE)][posZ - (blockZ * CHUNK_SIZE)]].solid == 1) {
			return 1;
		}
		return 0;
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
			break;
		case NTT:
			return (visibleEntity(posX, posY, posZ) != -1);
			break;
		case PRRT:
			return (isVisible(posX, posY, posZ, TRRN)) ? 1 : isVisible(posX, posY, posZ, NTT);
			break;
		}
	}
#ifdef _WIN32
	sf::Texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT) {

#else
	sf2d_texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT) {

#endif
		int blockX = floor(posX / CHUNK_SIZE);
		int blockY = floor(posY / CHUNK_SIZE);
		int blockZ = floor(posZ / CHUNK_SIZE);
		int chunkPosition = getChunkPos(blockX, blockY, blockZ);
		if (chunkPosition == -1) {
			for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX != -1; i++) {

				if (entityList[visibleEntity(posX, posY, posZ)].spriteName == texTable[i].name) {
#ifdef _WIN32
					return &texTable[i].texture;
#else 
					return texTable[i].texture;
#endif
				}
			}
		}
		switch (mode_t) {

		case TRRN:
#ifdef _WIN32
			if (isVisible(posX, posY, posZ, TRRN)) {
				return &texTable[getTexturePos(terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].textureFile)].texture;
			}
#else 
		if (isVisible(posX, posY, posZ, TRRN)) {
			return texTable[getTexturePos(terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].textureFile)].texture;
		}
#endif
		break;
		case NTT:
			for (int i = 0; i < ENTITY_LIST_SIZE && entityList[i].posX != -1; i++) {

				if (entityList[visibleEntity(posX, posY, posZ)].spriteName == texTable[i].name) {
#ifdef _WIN32
					return &texTable[i].texture;
#else 
					return texTable[i].texture;
#endif
				}
			}
			break;
			//cout<< "Entity texture not found in position" << posX << ' ' << posY << ' ' << posZ << endl;
		case PRRT:
			if (isVisible(posX, posY, posZ, NTT) == 1) {
#ifdef _WIN32
				return &texTable[getTexturePos(entityList[visibleEntity(posX, posY, posZ)].spriteName)].texture;
#else 
				return texTable[getTexturePos(entityList[visibleEntity(posX, posY, posZ)].spriteName)].texture;
#endif
			}

			else if (isVisible(posX, posY, posZ, TRRN)) {
				#ifdef _WIN32
				return &texTable[getTexturePos(terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].textureFile)].texture;

#else 
				return texTable[getTexturePos(terrainList[terrainMap[chunkPosition][posX - blockX * CHUNK_SIZE][posY - blockY * CHUNK_SIZE][posZ - blockZ * CHUNK_SIZE]].textureFile)].texture;

#endif
			}
			break;
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
#ifdef _WIN32
	sf::RenderWindow window;
	sf::Texture overlay;
#else
	u32 kDown;
	u32 kUp;
	u32 kHeld;
	sf2d_texture *overlay;
#endif

	gameMap map;
	entity* player;
	bool showGrid = 0;

	u64 loop = 0;
	bool autoJump = 0;
	int jumped = 0;
	void exitGame() {
#ifndef _WIN32
		
		csndExit();
		sf2d_fini();
		gfxExit();
		hidExit();
		aptExit();
		srvExit();
#endif

		exit(1);
	}
	void moveEntity(entity &currentEntity, direction dir, bool autojump = false) {
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
			if (currentEntity.posY > 8) {
				if (map.simpleCollision(currentEntity.posX, currentEntity.posY - 1, currentEntity.posZ) == 0) {
					currentEntity.posY--;
				}
				else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX, currentEntity.posY - 1, currentEntity.posZ + 1) == 0) {
					currentEntity.posY--;
					currentEntity.posZ++;
				}
			}
			else {
				cout << "BARRERA INVISBLE LOKO" << endl;
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
			if (currentEntity.posX > 16) {
				if (map.simpleCollision(currentEntity.posX - 1, currentEntity.posY, currentEntity.posZ) == 0) {
					currentEntity.posX--;
				}
				else if (autojump && map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ + 1) == 0 && map.simpleCollision(currentEntity.posX - 1, currentEntity.posY, currentEntity.posZ + 1) == 0) {
					currentEntity.posX--;
					currentEntity.posZ++;
				}
			}
			else {
				cout << "BARRERA INVISBLE LOKO" << endl;
			}
			break;
		case RIGHT:
			if (map.simpleCollision(currentEntity.posX + 1, currentEntity.posY, currentEntity.posZ) == 0) {
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
		if (map.simpleCollision(currentEntity.posX, currentEntity.posY, currentEntity.posZ - 1) == 0) {
			//cout<< "que es faci la gravetat " << endl;
			currentEntity.posZ--;
			if (currentEntity.posZ < 0) {
				cout << "has caigut del mon, capoll" << endl;
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
#ifdef _WIN32
		window.clear();
		//window.draw();
		sf::Sprite sprite[15][25];
		for (int i = 0; i != 15; i++) {
			for (int j = 0; j != 25; j++) {
				for (int y = RENDER_HEIGHT; y >= 0; y--) {
					if (map.isVisible((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - y),PRRT)) {
						//sprite[i][j].setTexture(map.getTexture((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - y)));
						sprite[i][j].setTexture(*map.getTexture((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - y),PRRT));

						sprite[i][j].setPosition(sf::Vector2f(j * 16, i * 16));
						window.draw(sprite[i][j]);
					}
				}
			}
		}
		if (showGrid) {
			//sf2d_draw_texture(overlay, 0, 0);
		}
		window.display();
#else
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		for (int i = 0; i != 15; i++) {
			for (int j = 0; j != 25; j++) {
				for (int y = RENDER_HEIGHT; y >= 0; y--) {
					if (map.isVisible((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - y),PRRT)) {
						sf2d_draw_texture(map.getTexture((player->posX + j) - 12, (player->posY + i) - 7, (player->posZ - y),PRRT), j * 16, i * 16);
					}
				}
			}
		}
		if (showGrid) {
			sf2d_draw_texture(overlay, 0, 0);
		}
		sf2d_end_frame();
#endif
	}
	void handleInput() {
#ifdef _WIN32
	
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			exitGame();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			autoJump = !autoJump;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			showGrid = 1;
		}
		else showGrid = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			moveEntity(*player, RIGHT, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			moveEntity(*player, LEFT, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			moveEntity(*player, FRONT, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			moveEntity(*player, BACK, autoJump);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (!jumped) {
				moveEntity(*player, UP, autoJump);
				moveEntity(*player, UP, autoJump);
				jumped = 2;
			}
		}
#else

	hidScanInput();
	kDown = hidKeysDown();
	kHeld = hidKeysHeld();
	if (kDown & KEY_START) {
		exitGame();
	}
	if (kDown & KEY_X) {
		autoJump = !autoJump;
	}
	if (kHeld & KEY_SELECT) {
		showGrid = 1;
	}
	else showGrid = 0;
	if (kHeld & KEY_RIGHT) {
		moveEntity(*player, RIGHT, autoJump);
	}
	if (kHeld & KEY_LEFT) {
		moveEntity(*player, LEFT, autoJump);
	}
	if (kHeld & KEY_UP) {
		moveEntity(*player, FRONT, autoJump);
	}
	if (kHeld & KEY_DOWN) {
		moveEntity(*player, BACK, autoJump);
	}
	if (kHeld & KEY_A) {
		if (!jumped) {
			moveEntity(*player, UP, autoJump);
			moveEntity(*player, UP, autoJump);
			jumped = 2;
		}
	}
#endif
	}
void gameLoop() {
	//cout<< "loop." << loop;

	point3D playerPos;
	playerPos.x = player->posX;
	playerPos.y = player->posY;
	playerPos.z = player->posZ;
	if (loop % 10 == 0) {
		handleInput();
		updateEntities();
	}
	map.loadNewChunk(playerPos);
	if (jumped > 0) { jumped--; }

	//cout<< player->posX << ' ' << player->posY << ' ' << player->posZ << endl;
#ifdef _WIN32
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
	}
#else 
	cout << sf2d_get_fps()<<endl;
	sf2d_swapbuffers();
#endif


	//processar input
	//refrescar coses carregades
	//dibuixar

	drawFrame();
}
public:
	void gameCore() {
#ifdef _WIN32
		window.create(sf::VideoMode(400, 240), "RoguePC");
		sf::Music music;
		music.openFromFile("data/sounds/bgm/wilderness.ogg");
		music.play();
#endif
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


		//overlay = sfil_load_PNG_file("data/sprites/overlay.png", SF2D_PLACE_RAM);
		
		player = &map.entityList[0];
		string saveName = "default";
		ifstream general;
		general.open("saves/" + saveName + "/general.txt");
		if (!general.is_open()) {
			//cout<< "couldn't open file: " << ("saves/" + saveName + "/general.txt") << endl;
		}
		string playerSprite, playerName;
		general >> playerName >> player->spriteName >> player->posX >> player->posY >> player->posZ;
		//cout<< playerName << endl;
		point3D playerPos;
		//cout<< player->posX << endl;
		playerPos.x = player->posX;
		playerPos.y = player->posY;
		playerPos.z = player->posZ;
		//loadmap
		for (int i = 0; i < CHUNK_NUM; i++) {
			map.loadNewChunk(playerPos);
		}
		map.loadTexture("player.png");
		map.loadTerrainTable();
		//cout<< "vram: " << vramSpaceFree() << endl;
		//cout<< "mappable: " << mappableSpaceFree() << endl;
		//cout<< "linear: " << linearSpaceFree() << endl;
		//audio_load("data/sounds/bgm/wilderness.raw"); //[N3DS] only 
		while (1) {
			gameLoop();
			loop++;
		}
	}
};

int main()
{
#ifndef _WIN32
	srvInit();
	aptInit();
	hidInit();
	srand(time(NULL));
	sf2d_init();
	consoleInit(GFX_BOTTOM, NULL);
	csndInit();
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
#endif
	srand(time(NULL));
	gameMain gameMain1;
	gameMain1.gameCore();
#ifndef _WIN32
	
	csndExit();


		// Exit
		//map.freeAllTextures();

		sf2d_fini();

	gfxExit();

	hidExit();

	aptExit();

	srvExit();
#endif
	return 0;
}