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

	terrain terrainList[TERRAIN_LIST_SIZE]; //Llista de tipus de terrenys diferents, aqui �s on es mirar� a partir del terrainMap
	point3D mapIndex[CHUNK_NUM]; //indica quin bloc de terreny hi ha a cada posici�		
	textureName texTable[TEX_TABLE_SIZE];
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
	bool isTextureLoaded(string textureFile);
	int freeTexturePos();
	int getTexturePos(string fileName);
	void loadTexture(string fileName);
	void freeTexture(string fileName);
	void freeAllTextures();
	bool simpleCollision(int posX, int posY, int posZ, mode collisionMode = TRRN);
	int visibleEntity(int posX, int posY, int posZ);
	bool isVisible(int posX, int posY, int posZ, mode mode_t = PRRT);
#ifdef _WIN32
	sf::Texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT);

#else
	sf2d_texture* getTexture(int posX, int posY, int posZ, mode mode_t = PRRT);

#endif
		
	gameMap();
};