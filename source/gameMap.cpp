#include "../include/gameMap.h"
#include "../include/core.h"
#include "../include/FastNoise.h"
#include "../include/HardwareInterface.h"

using namespace std;

void gameMap::exit() {
	threadCloseRequest = true;
	while (threadStatus) {
		HI::sleepThread(10000000);
	}
	freeAllChunks();
	for (int i = 0; i < CHUNK_NUM; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				delete terrainMap[i][j][k];
			}
			delete terrainMap[i][j];
		}
		delete terrainMap[i];
	}
	delete terrainMap;
}

void gameMap::chunkLoader(HI::HISize temp, void* arg) {
	chunkLoader(arg);
}

void gameMap::chunkLoader(void* arg) {
	HI::debugPrint("SOC AL THREAD \n", 5);
	gameMap* gameObj = (gameMap*)arg;
	while (!gameObj->threadCloseRequest) {
		HI::debugPrint("SOC AL BUCLE DEL THREAD \n",5);
		gameObj->loadNewChunk();
		HI::sleepThread(50000000);
	}
	gameObj->threadStatus = false;
}

void gameMap::startChunkLoader() {
	threadStatus = true;
	if (HI::getConsole() == HI::CONSOLE_PSVITA) {
		HI::createThread((void*)static_cast<void(*)(HI::HISize, void*)>(chunkLoader), this, 50000, 0x3F, 0, true, sizeof(*this));
	}
	else if(HI::getConsole() == HI::CONSOLE_NINTENDO3DS)
	{
		HI::createThread((void*)static_cast<void(*)(void*)>(chunkLoader), this, 5000, 0x3F, 0, true, sizeof(*this));
	}
}

point3D gameMap::getChunk(point3D pos) {
	HI::debugPrint("LoadNewChunk231 \n", 5);
	point3D b;
	b.x = pos.x / CHUNK_SIZE;
	b.y = pos.y / CHUNK_SIZE;
	b.z = pos.z / CHUNK_SIZE;
	HI::debugPrint("LoadNewChunk123 \n", 5);
	return b;
}

point3D gameMap::getChunk(int x, int y, int z) {
	point3D b;
	b.x = x / CHUNK_SIZE;
	b.y = y / CHUNK_SIZE;
	b.z = z / CHUNK_SIZE;
	return b;
}

unsigned char* gameMap::getBlock(point3D posBlock) const {
	point3D chunkPos = getChunk(posBlock);
	int chunkID = getChunkID(chunkPos);
	return &terrainMap[chunkID][posBlock.x - (chunkPos.x * CHUNK_SIZE)][posBlock.y - (chunkPos.y * CHUNK_SIZE)][posBlock.z - (chunkPos.z * CHUNK_SIZE)];
}


void gameMap::putBlock(int block, point3D posBlock) {
	*getBlock(posBlock) = block;
}

void gameMap::createMapAndLoad(unsigned char*** map, point3D c, FastNoise noiseObj) {
	//cout<< "intento crear un chunk a " << chunkX << chunkY << chunkZ << endl;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			int terrainHeight = FLOOR_HEIGHT + 100 * (1 + noiseObj.GetNoise(c.x * CHUNK_SIZE + j, c.y * CHUNK_SIZE + i) / 2);
			for (int h = 0; h < CHUNK_SIZE; h++) {
				if (h + c.z*CHUNK_SIZE > terrainHeight) {	//si esta per sobre la terra
					if (h + c.z * CHUNK_SIZE <= SEA_LEVEL) map[j][i][h] = 3;
					else map[j][i][h] = 0;
				}
				else if (h + c.z*CHUNK_SIZE == terrainHeight) {	 //si es la capa superficial
					if (terrainHeight <= SEA_LEVEL) map[j][i][h] = 4;
					else map[j][i][h] = (rand() % 15 == 0 ? rand() % 14 == 0 ? 7 : 6 : 1);
				}
				else map[j][i][h] = 2;
			}
		}
	}
}

int gameMap::chunkValue(point3D chunkN, point3D chunkO) const {
	if (chunkN.x < 0 || chunkN.y < 0 || chunkN.z < 0) return 32;
	return (pow(chunkN.x - chunkO.x, 2) + pow(chunkN.y - chunkO.y, 2) + pow(chunkN.z - chunkO.z, 2));
}
int gameMap::freeChunkID() const { //returns the position inside mapIndex[] of the first free chunk space
	for (int i = 0; i < CHUNK_NUM; i++) {
		if (mapIndex[i].x == -1) {
			return i;
		}
	}
	//cout<< "NO FREE SPACE IN MAPINDEX" << endl;
	return -1;
}
int gameMap::getChunkID(point3D p) const { //returns the position inside mapIndex of the aforementioned chunk;
	if (p.x >= 0 && p.y >= 0 && p.z >= 0) {
		for (int i = 0; i < CHUNK_NUM; i++) {
			if (p.x == mapIndex[i].x) {
				if (p.y == mapIndex[i].y) {
					if (p.z == mapIndex[i].z) {
						return i;
					}
				}
			}
		}
	}
	return -1;
}
int gameMap::getBlocksChunkID(point3D b) const { //returns the position inside mapIndex of the aforementioned chunk;
	point3D p = getChunk(b);
	for (int i = 0; i < CHUNK_NUM; i++) {
		if (p.x == mapIndex[i].x) {
			if (p.y == mapIndex[i].y) {
				if (p.z == mapIndex[i].z) {
					return i;
				}
			}
		}
	}
	return -1;
}

terrain gameMap::getTerrain(point3D pos) {
	point3D chunk = getChunk(pos);
	if (!isChunkLoaded(chunk)) {
		return terrain();//cout << "Tried to get terrain from unloaded chunk"<<pos.x<<' '<<pos.y<<' ' <<pos.z <<endl;
	}
	return terrainList[terrainMap[getChunkID(chunk)][pos.x % CHUNK_SIZE][pos.y % CHUNK_SIZE][pos.z % CHUNK_SIZE]];
}

void gameMap::addPlayer(point3D* pos) {
	playerPos = pos;
}

bool gameMap::isChunkLoaded(point3D p) const { // tells if said chunk is loaded in mapIndex[]
	for (int i = 0; i < CHUNK_NUM; i++) {
		if (mapIndex[i].z == p.z) {
			if (mapIndex[i].x == p.x) {
				if (mapIndex[i].y == p.y) {
					return true;
				}
			}
		}
	}
	return false;
}
void gameMap::saveChunk(point3D c) { //unloads a chunk from memory and saves it in its file
	//cout<< "saving chunk " << chunkX << chunkY << chunkZ << endl;
	int chunkPos = getChunkID(c);
	if (chunkPos == -1) {
		cout << "chunk is already unloaded" << endl;
		return;
	}
	ofstream chunkFile;
	string terrainName = (HI::getSavesPath() + saveName + "/chunks/terrain." + get_string(c.x) + '.' + get_string(c.y) + '.' + get_string(c.z));
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
void gameMap::freeAChunk() {
	point3D playerChunk = getChunk(*playerPos);
	for (int i = 0; i < CHUNK_NUM; i++) {
		point3D chunkN = mapIndex[i];
		if (chunkValue(chunkN, playerChunk) > 3) {
			saveChunk(chunkN);
			//cout<< "                 FREE " << (float)(clock() - t) / CLOCKS_PER_SEC * 1000 << endl;
			return;
		}
	}
}

void gameMap::freeAllChunks() {
	for (int i = 0; i < CHUNK_NUM; i++) {
		point3D chunkN = mapIndex[i];
		if (chunkN.x != -1) {
			saveChunk(chunkN);
		}
	}
}

void gameMap::loadChunk(point3D c) { //Loads a certain chunk inside mapIndex[] and terrainMap[][][][];
	int chunkPos = freeChunkID();
	if (chunkPos == -1) {
		freeAChunk();
		chunkPos = freeChunkID();
	}
	ifstream chunkFile;
	string terrainName = (HI::getSavesPath() + saveName + "/chunks/terrain." + get_string(c.x) + '.' + get_string(c.y) + '.' + get_string(c.z));
	chunkFile.open(terrainName, ios_base::binary);
	if (!chunkFile.is_open()) {
		//cout<< "couldn't open file: " << terrainName << endl;
		createMapAndLoad(terrainMap[chunkPos], c, noiseObj);
		mapIndex[chunkPos].x = c.x;
		mapIndex[chunkPos].y = c.y;
		mapIndex[chunkPos].z = c.z;
		return;
		//chunkFile.open(terrainName, ios_base::binary);
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
	chunkFile.close();
	mapIndex[chunkPos].x = c.x;
	mapIndex[chunkPos].y = c.y;
	mapIndex[chunkPos].z = c.z;
}
void gameMap::loadTerrainTable() {
	ifstream terrainTable;
	string terrainName = (HI::getSavesPath() + saveName + "/terrainTable.txt");
	terrainTable.open(terrainName);
	if (!terrainTable.is_open()) {
		HI::debugPrint("error opening terrainTable \n");
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
void gameMap::loadNewChunk() {
	HI::debugPrint("LoadNewChunk \n", 5);

	point3D playerChunk = getChunk(*playerPos);
	HI::debugPrint("LoadNewChunk9 \n", 5);
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			for (int k = -1; k < 2; k++) {
				HI::debugPrint("LoadNewChunk1 \n", 5);
				point3D chunkPos;
				chunkPos.x = playerChunk.x + i;
				chunkPos.y = playerChunk.y + j;
				chunkPos.z = playerChunk.z + k;
				if (chunkPos.x >= 0 && chunkPos.y >= 0 && chunkPos.z >= 0) {
					if (chunkValue(chunkPos, playerChunk) <= 3) {
						if (isChunkLoaded(chunkPos) == 0) {
							HI::debugPrint("trying to load chunk \n", 6);
							if (freeChunkID() == -1) {
								HI::debugPrint("LoadNewChunk2 \n", 5);
								freeAChunk();	HI::debugPrint("LoadNewChunk3 \n", 5);
								if (freeChunkID() == -1) {
									HI::debugPrint("No free space for a new chunk \n",6);
									return;
								}
							}
							HI::debugPrint("LoadNewChunk4 \n", 5);
							loadChunk(chunkPos);
							HI::debugPrint("LoadNewChunk5 \n", 5);
							return;
						}
					}
				}
			}
		}
	}
}
bool gameMap::simpleCollision(int posX, int posY, int posZ) const {	//Tells if terrain at position is occupied
	point3D b;
	b.x = posX;
	b.y = posY;
	b.z = posZ;
	return simpleCollision(b);
}
bool gameMap::simpleCollision(point3D p) const {	//Tells if terrain at position is occupied
	if (p.x <= 0 || p.y <= 0 || p.z <= 0) return true;
	if (isChunkLoaded(getChunk(p)) && terrainList[*getBlock(p)].solid) {
		return (isChunkLoaded(getChunk(p)) && terrainList[*getBlock(p)].solid);
	}
	return false;
}

bool gameMap::isVisible(point3D p) const { //
	point3D b;
	b.x = floor(p.x / CHUNK_SIZE);
	b.y = floor(p.y / CHUNK_SIZE);
	b.z = floor(p.z / CHUNK_SIZE);
	int chunkPosition = getChunkID(b);
	if (chunkPosition == -1) {
		//HI::debugPrint("gameMap::isVisible chunk not loaded \n");			   molt costós
		return false;
	}
	return terrainList[terrainMap[chunkPosition][p.x - b.x * CHUNK_SIZE][p.y - b.y * CHUNK_SIZE][p.z - b.z * CHUNK_SIZE]].visible;
}
int gameMap::getTerrainListSize() const {
	return terrainListSize;
}
bool gameMap::isVisible(int n) const {
	return terrainList[n].visible;
}
string gameMap::getTextureName(int n) const {
	return terrainList[n].textureFile;
}

int gameMap::getTerrainListPos(point3D p) const {
	point3D b;
	b.x = floor(p.x / CHUNK_SIZE);
	b.y = floor(p.y / CHUNK_SIZE);
	b.z = floor(p.z / CHUNK_SIZE);
	int chunkPosition = getChunkID(b);
	if (chunkPosition == -1) {
		HI::debugPrint("gM::gTLP No s ha trobat el chunk \n");
	}
	return terrainMap[chunkPosition][p.x - b.x * CHUNK_SIZE][p.y - b.y * CHUNK_SIZE][p.z - b.z * CHUNK_SIZE];
}

string gameMap::getTerrainName(point3D p) const {
	return terrainList[getTerrainListPos(p)].textureFile;
}

bool gameMap::getTerrainVisible(point3D p) const {
	return terrainList[getTerrainListPos(p)].visible;
}

bool gameMap::getTerrainSolid(point3D p) const {
	return terrainList[getTerrainListPos(p)].solid;
}

gameMap::gameMap(string nameString) {
	noiseObj.SetNoiseType(FastNoise::SimplexFractal);
	noiseObj.SetFractalOctaves(5);
	noiseObj.SetSeed(0);
	noiseObj.SetFrequency(0.003f);
	noiseObj.SetFractalLacunarity(2);
	noiseObj.SetFractalGain(1 / 2);
	terrainListSize = 0;
	saveName = nameString;
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
	threadStatus = false;
	threadCloseRequest = false;
}