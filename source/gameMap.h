#pragma once
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#ifndef _WIN32
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#endif
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

	void createMapAndLoad(unsigned char*** map, int chunkX, int chunkY, int chunkZ);
	int chunkValue(point3D chunkN, point3D chunkO);
	int freeChunkPos();
	int getChunkPos(int posX, int posY, int posZ);
	bool isChunkLoaded(int posX, int posY, int posZ);
	void saveChunk(int chunkX, int chunkY, int chunkZ);
	void freeAChunk(point3D playerPos);
	void loadChunk(int chunkX, int chunkY, int chunkZ, point3D playerPos);
	void loadTerrainTable();
	void loadNewChunk(point3D playerPos);
	int getTerrainListSize();
	bool simpleCollision(int posX, int posY, int posZ, mode collisionMode = TRRN);
	int visibleEntity(int posX, int posY, int posZ);
	bool isVisible(int posX, int posY, int posZ, mode mode_t = PRRT);
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
