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
		terrainListSize++;
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
int gameMap::getTerrainListSize() {
	return terrainListSize;
}
bool gameMap::isVisible(int n) {
	return terrainList[n].visible;
}
string gameMap::getTextureName(int n) {
	return terrainList[n].textureFile;
}

int gameMap::getTerrainListPos(point3D p) {
	int blockX = floor(p.x / CHUNK_SIZE);												   //AKESTA FUNCIO ES EL PUTO SIDA, I DEMOSTRA QUE HI HA MOLTA COSA A CANVIAR, MOLTISSIMA
	int blockY = floor(p.y / CHUNK_SIZE);
	int blockZ = floor(p.z / CHUNK_SIZE);
	int chunkPosition = getChunkPos(blockX, blockY, blockZ);
	if (chunkPosition == -1) {
		cout << "NO S'HA TROBAT EL CHUNK" << endl;
	}
	return terrainMap[chunkPosition][p.x - blockX * CHUNK_SIZE][p.y - blockY * CHUNK_SIZE][p.z - blockZ * CHUNK_SIZE];
}

string gameMap::getTerrainName(point3D p)
{
	return terrainList[getTerrainListPos(p)].textureFile;
}

bool gameMap::getTerrainVisible(point3D p)
{
	return terrainList[getTerrainListPos(p)].visible;
}

bool gameMap::getTerrainSolid(point3D p)
{
	return terrainList[getTerrainListPos(p)].solid;
}

std::string gameMap::getEntityName(point3D p)
{
	return (entityList[visibleEntity(p.x, p.y, p.z)].spriteName);
}

bool gameMap::getEntityVisible(point3D p)
{
	return (entityList[visibleEntity(p.x, p.y, p.z)].visible);
}

bool gameMap::getEntitySolid(point3D p)
{
	return (entityList[visibleEntity(p.x, p.y, p.z)].solid);
}

gameMap::gameMap() {
	terrainListSize = 0;
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