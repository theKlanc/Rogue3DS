 #pragma once
#include <sf2d.h>
#include "gameMap.h"
#include "core.h"

class graphics
{
private:
	entity* player;
	gameMap* mapObj;
	textureName texTable[TEX_TABLE_SIZE];
	bool isTextureLoaded(string textureFile);
	int freeTexturePos();
	int getTexturePos(string fileName);
	void loadTexture(string fileName);
	void freeTexture(string fileName);
	void freeAllTextures();
	sf2d_texture* getTexture(point3D p, mode mode_t = PRRT);
public:
	graphics();
	graphics(gameMap &map, entity &playerOrig);
	void edit(gameMap &map, entity &playerOrig);
	void drawFrame();
	void reloadTextures();

};