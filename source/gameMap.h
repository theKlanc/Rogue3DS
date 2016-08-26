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
	bool threadStatus;
	bool threadCloseRequest;
	static void chunkLoader(u32 arg);
	unsigned char* getBlock(point3D posBlock) const;
	void putBlock(int block, point3D posBlock);
	static void createMapAndLoad(unsigned char*** map, point3D c);
	int chunkValue(point3D chunkN, point3D chunkO) const;
	int freeChunkID() const;
	int getBlocksChunkID(point3D b) const;
	void saveChunk(point3D c);
	void freeAChunk(point3D playerPos);
	void freeAllChunks();

	void loadChunk(point3D c, point3D playerPos);
	int visibleEntity(point3D p) const;
	int getTerrainListPos(point3D p) const;
	bool getTerrainVisible(point3D p) const;
	bool getTerrainSolid(point3D p) const;

	bool getEntityVisible(point3D p) const;
	bool getEntitySolid(point3D p) const;
public:

	bool isChunkLoaded(point3D p) const;
	static point3D getChunk(point3D pos);
	static point3D getChunk(int x, int y, int z);
	int getChunkID(point3D p) const;
	string getEntityName(point3D p) const;
	string getTerrainName(point3D p) const;
	int getTerrainListSize() const;
	string getTextureName(int n) const;
	bool isVisible(point3D p, mode mode_t = PRRT) const;
	bool isVisible(int n) const;
	void exit();
	void startChunkLoader(point3D* temp1);
	void loadTerrainTable();
	void loadNewChunk(point3D playerPos);
	entity entityList[ENTITY_LIST_SIZE]; //Processats individualment cada frame // HAURIA D USAR UN STD::VECTOR PER PODERLOS REORDENAR
	bool simpleCollision(point3D p, mode collisionMode = TRRN) const;
	bool simpleCollision(int posX, int posY, int posZ, mode collisionMode = TRRN) const;
	gameMap(string nameString);
};