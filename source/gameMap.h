#pragma once
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <stdlib.h> 
#include "core.h"

using namespace std;

class gameMap {
private:
	unsigned char**** terrainMap;

	terrain terrainList[TERRAIN_LIST_MAX_SIZE]; //Llista de tipus de terrenys diferents, aqui és on es mirarà a partir del terrainMap
	int terrainListSize;
	point3D mapIndex[CHUNK_NUM]; //indica quin bloc de terreny hi ha a cada posició		
	
	string saveName;
	Thread threadHandle;
	volatile bool threadStatus;
	volatile bool threadCloseRequest;
	static void chunkLoader(u32 arg);
	unsigned char* getBlock(point3D posBlock);
	void putBlock(int block, point3D posBlock);
	void createMapAndLoad(unsigned char*** map, point3D c);
	int chunkValue(point3D chunkN, point3D chunkO);
	int freeChunkID();
	int getBlocksChunkID(point3D b);
	void saveChunk(point3D c);
	void freeAChunk(point3D playerPos);
	void freeAllChunks();

	void loadChunk(point3D c, point3D playerPos);
	int visibleEntity(point3D p);
	int getTerrainListPos(point3D p);
	bool getTerrainVisible(point3D p);
	bool getTerrainSolid(point3D p);

	bool getEntityVisible(point3D p);
	bool getEntitySolid(point3D p);
public:

	bool isChunkLoaded(point3D p);
	point3D getChunk(point3D pos);
	point3D getChunk(int x, int y, int z);
	int getChunkID(point3D p);
	string getEntityName(point3D p);
	string getTerrainName(point3D p);
	int getTerrainListSize();
	string getTextureName(int n);
	bool isVisible(point3D p, mode mode_t = PRRT);
	bool isVisible(int n);
	void exit();
	void startChunkLoader(point3D* temp1);
	void loadTerrainTable();
	void loadNewChunk(point3D playerPos);
	entity entityList[ENTITY_LIST_SIZE]; //Processats individualment cada frame // HAURIA D USAR UN STD::VECTOR PER PODERLOS REORDENAR
	bool simpleCollision(point3D p, mode collisionMode = TRRN);
	bool simpleCollision(int posX, int posY, int posZ, mode collisionMode = TRRN);
	gameMap(string nameString);
};
struct threadArg1 {
	gameMap* map;
	point3D* player;
	volatile bool* threadExitReq;
	volatile bool* threadState;
};
