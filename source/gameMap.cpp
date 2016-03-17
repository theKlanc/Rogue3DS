#include "gameMap.h"
#include "common.h"

using namespace std;

void gameMap::createMapAndLoad(unsigned char*** map, int chunkX, int chunkY, int chunkZ) {
	//cout<< "intento crear un chunk a " << chunkX << chunkY << chunkZ << endl;
	if (chunkZ < FLOOR_HEIGHT / CHUNK_SIZE) {
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					map[i][j][k] = 1;
				}
			}
		}
	}
	else if (chunkZ == FLOOR_HEIGHT / CHUNK_SIZE) {
		for (int n = 0; n < CHUNK_SIZE; n++) {
			for (int m = 0; m < CHUNK_SIZE; m++) {
				for (int l = 0; l < CHUNK_SIZE; l++) {
					if (n < CHUNK_SIZE / 2) {
						map[l][m][n] = 1;
					}
					else if (n == CHUNK_SIZE / 2) {
						int bloc = rand() % 7;
						if (bloc == 1) { bloc++; }
						else bloc = 0;
						map[l][m][n] = bloc;
					}
					else {
						map[l][m][n] = 0;
					}
				}
			}
		}
	}
	else {
		for (int i = 0; i < CHUNK_SIZE; i++) {
			for (int j = 0; j < CHUNK_SIZE; j++) {
				for (int k = 0; k < CHUNK_SIZE; k++) {
					map[i][j][k] = 0;
				}
			}
		}
	}
}
int gameMap::chunkValue(point3D chunkN, point3D chunkO) {
	int res = (pow(chunkN.x - chunkO.x, 2) + pow(chunkN.y - chunkO.y, 2) + pow(chunkN.z - chunkO.z, 2));
	return res;
}
int gameMap::freeChunkPos() { //returns the position inside texTable[] of the first free texture space
	for (int i = 0; i < CHUNK_NUM; i++) {
		if (mapIndex[i].x == -1) {
			return i;
		}
	}
	//cout<< "NO FREE SPACE IN MAPINDEX" << endl;
	return -1;
}
int gameMap::getChunkPos(int posX, int posY, int posZ) { //returns the position inside mapIndex of the aforementioned chunk;
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
bool gameMap::isChunkLoaded(int posX, int posY, int posZ) { // tells if said chunk is loaded in mapIndex[]
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
void gameMap::saveChunk(int chunkX, int chunkY, int chunkZ) { //unloads a chunk from memory and saves it in its file
	cout << "SAVING";
	//cout<< "saving chunk " << chunkX << chunkY << chunkZ << endl;
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
	cout << " END" << endl;
	//cout<< "                 SAVE " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
	//cout<< "done saving" << endl;
}
void gameMap::freeAChunk(point3D playerPos) {
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
void gameMap::loadChunk(int chunkX, int chunkY, int chunkZ, point3D playerPos) { //Loads a certain chunk inside mapIndex[] and terrainMap[][][][];
	int chunkPos = freeChunkPos();
	if (chunkPos == -1) {
		freeAChunk(playerPos);
		chunkPos = freeChunkPos();
	}
	mapIndex[chunkPos].x = chunkX;
	mapIndex[chunkPos].y = chunkY;
	mapIndex[chunkPos].z = chunkZ;
	ifstream chunkFile;
	string terrainName = ("saves/" + saveName + "/chunks/terrain." + get_string(chunkX) + '.' + get_string(chunkY) + '.' + get_string(chunkZ));
	chunkFile.open(terrainName, ios_base::binary);
	if (!chunkFile.is_open()) {
		cout << "CREATING";
		//cout<< "couldn't open file: " << terrainName << endl;
		createMapAndLoad(terrainMap[chunkPos], chunkX, chunkY, chunkZ);
		cout << " END" << endl;
		return;
		//chunkFile.open(terrainName, ios_base::binary);
	}
	cout << "LOADING";
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
	cout << " END" << endl;
	//cout<< "                 LOAD " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
}
void gameMap::loadTerrainTable() {
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
void gameMap::loadNewChunk(point3D playerPos) {
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
bool gameMap::isTextureLoaded(string textureFile) { // tells if a texture with said name is present on texTable
	for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
		if (texTable[i].name == textureFile) {
			return 1;
		}
	}
	return 0;
}
int gameMap::freeTexturePos() { //returns the position inside texTable[] of the first free texture space
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		if (texTable[i].name == "free") {
			return i;
		}
	}
	//cout<< "NO FREE SPACE IN TEXTABLE" << endl;
	return -1;
}
int gameMap::getTexturePos(string fileName) { //returns the position inside texTable[] of the texture with said filename
	for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
		if (texTable[i].name == fileName) {
			return i;
		}
	}
	//cout<< "NO TEXTURE W/ FNAME " << fileName << " FOUND" << endl;
	return -1;
}
void gameMap::loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
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
void gameMap::freeTexture(string fileName) { //frees a texture from texTable[]
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
void gameMap::freeAllTextures() {	 //frees all textures
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		texTable[i].name = "free";
#ifndef _WIN32
		sf2d_free_texture(texTable[i].texture);
#endif
	}
}
bool gameMap::simpleCollision(int posX, int posY, int posZ, mode collisionMode) {	//Tells if terrain at position is occupied
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
int gameMap::visibleEntity(int posX, int posY, int posZ) { // returns the position inside entityList if the entered 3d position contains a visible entity
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
bool gameMap::isVisible(int posX, int posY, int posZ, mode mode_t) { //
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
sf::Texture* gameMap::getTexture(int posX, int posY, int posZ, mode mode_t) {

#else
sf2d_texture* gameMap::getTexture(int posX, int posY, int posZ, mode mode_t) {

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
gameMap::gameMap() {
	saveName = "default";
	terrainMap = new unsigned char***[CHUNK_NUM];
	for (int i = 0; i < CHUNK_NUM; i++) {
		terrainMap[i] = new unsigned char**[CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++) {
			terrainMap[i][j] = new unsigned char*[CHUNK_SIZE];
			for (int k = 0; k < CHUNK_SIZE; k++) {
				terrainMap[i][j][k] = new unsigned char[CHUNK_SIZE];
				for (int l = 0; l < CHUNK_SIZE; l++) {
					terrainMap[i][j][k][l] = 1;
				}
			}
		}
	}
}