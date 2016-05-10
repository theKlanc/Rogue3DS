#pragma once
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <stdlib.h> 
#include "common.h"

using namespace std;

class gameMap {
private:
	unsigned char**** terrainMap;

	terrain terrainList[TERRAIN_LIST_MAX_SIZE]; //Llista de tipus de terrenys diferents, aqui és on es mirarà a partir del terrainMap
	int terrainListSize;
	point3D mapIndex[CHUNK_NUM]; //indica quin bloc de terreny hi ha a cada posició		
	
	string saveName;
public:
	entity entityList[ENTITY_LIST_SIZE]; //Processats individualment cada frame // HAURIA D USAR UN STD::VECTOR PER PODERLOS REORDENAR
	point3D getChunk(point3D pos);
	unsigned char* getBlock(point3D posBlock);
	void putBlock(int block, point3D posBlock);
	void createMapAndLoad(unsigned char*** map, point3D c);
	int chunkValue(point3D chunkN, point3D chunkO);
	int freeChunkID();
	int getChunkID(point3D p);
	int getBlocksChunkID(point3D b);
	bool isChunkLoaded(point3D p);
	void saveChunk(point3D c);
	void freeAChunk(point3D playerPos);
	void freeAllChunks();

	void loadChunk(point3D c, point3D playerPos);
	void loadTerrainTable();
	void loadNewChunk(point3D playerPos);
	int getTerrainListSize();
	bool simpleCollision(point3D p, mode collisionMode = TRRN);
	bool simpleCollision(int posX,int posY, int posZ, mode collisionMode = TRRN);
	int visibleEntity(point3D p);
	bool isVisible(point3D p, mode mode_t = PRRT);
	bool isVisible(int n);
	string getTextureName(int n);
	int getTerrainListPos(point3D p);
	string getTerrainName(point3D p);
	bool getTerrainVisible(point3D p);
	bool getTerrainSolid(point3D p);

	string getEntityName(point3D p);
	bool getEntityVisible(point3D p);
	bool getEntitySolid(point3D p);
	gameMap();
};
