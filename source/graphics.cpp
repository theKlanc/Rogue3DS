#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include "gameMap.h"
#include "graphics.h"

using namespace std;

graphics::graphics() {
	loadTexture("player.png");
	downTexture = sfil_load_PNG_file("data/sprites/down.png", SF2D_PLACE_RAM);
	upTexture = sfil_load_PNG_file("data/sprites/up.png", SF2D_PLACE_RAM);	
}
graphics::graphics(gameMap &map, entity &playerOrig) {
	loadTexture("player.png");
	mapObj = &map;
	player = &playerOrig;
}

void graphics::edit(gameMap &map, entity &playerOrig) {
	mapObj = &map;
	player = &playerOrig;
}

void graphics::drawFrame() {
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	point3D p;
	for (int i = 0; i != 15; i++) {
		for (int j = 0; j != 25; j++) {
			for (int y = RENDER_HEIGHT; y >= 0; y--) {
				p.x = (player->pos.x + j) - 12;
				p.y = (player->pos.y + i) - 7;
				p.z = (player->pos.z - y);
				if (mapObj->isVisible(p, PRRT)) {
					sf2d_draw_texture(getTexture(p, PRRT), j * 16, i * 16);
					if (y > 1) sf2d_draw_texture(downTexture, j * 16, i * 16);
					else if (y == 0) sf2d_draw_texture(upTexture, j * 16, i * 16);
				}
			}
		}
	}
	sf2d_end_frame();
}

bool graphics::isTextureLoaded(string textureFile) { // tells if a texture with said name is present on texTable
	for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
		if (texTable[i].name == textureFile) {
			return 1;
		}
	}
	return 0;
}
int graphics::freeTexturePos() { //returns the position inside texTable[] of the first free texture space
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		if (texTable[i].name == "free") {
			return i;
		}
	}
	//cout<< "NO FREE SPACE IN TEXTABLE" << endl;
	return -1;
}
int graphics::getTexturePos(string fileName) { //returns the position inside texTable[] of the texture with said filename
	for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != "free"; i++) {
		if (texTable[i].name == fileName) {
			return i;
		}
	}
	//cout<< "NO TEXTURE W/ FNAME " << fileName << " FOUND" << endl;
	return -1;
}
void graphics::loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
	int freeTextureLoc = freeTexturePos();
	texTable[freeTextureLoc].name = fileName;
	fileName = "data/sprites/" + fileName;
	texTable[freeTextureLoc].texture = sfil_load_PNG_file(fileName.c_str(), SF2D_PLACE_RAM);
	sf2d_texture_tile32(texTable[freeTextureLoc].texture);

}
void graphics::freeTexture(string fileName) { //frees a texture from texTable[]
	int textureLocation = getTexturePos(fileName);
	int freeTexLoc = freeTexturePos();
	textureName temp = texTable[textureLocation];
	texTable[textureLocation] = texTable[freeTexLoc - 1];
	texTable[freeTexLoc - 1] = temp;
	texTable[freeTexLoc - 1].name = "free";

	sf2d_free_texture(texTable[freeTexLoc - 1].texture);

}
void graphics::freeAllTextures() {	 //frees all textures
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		texTable[i].name = "free";

		sf2d_free_texture(texTable[i].texture);

	}
}


sf2d_texture* graphics::getTexture(point3D p, mode mode_t) {

	point3D b;
	b.x = floor(p.x / CHUNK_SIZE);												   //AKESTA FUNCIO ES EL PUTO SIDA, I DEMOSTRA QUE HI HA MOLTA COSA A CANVIAR, MOLTISSIMA
	b.y = floor(p.y / CHUNK_SIZE);
	b.z = floor(p.z / CHUNK_SIZE);

	int chunkPosition = mapObj->getChunkID(b);
	switch (mode_t) {
	case TRRN:
		if (chunkPosition == -1) {
			getTexture(p, NTT);
		}
		if (mapObj->isVisible(p, TRRN)) {
			return texTable[getTexturePos(mapObj->getTerrainName(p))].texture;
		}
		break;
	case NTT:
		for (int i = 0; i < ENTITY_LIST_SIZE && mapObj->entityList[i].pos.x != -1; i++) {
			if (mapObj->getEntityName(p) == texTable[i].name) {
				return texTable[i].texture;
			}
		}
		break;
		//cout<< "Entity texture not found in position" << posX << ' ' << posY << ' ' << posZ << endl;
	case PRRT:
		if (chunkPosition == -1) {
			getTexture(p, NTT);
		}
		if (mapObj->isVisible(p, NTT) == 1) {
			for (int i = 0; i < ENTITY_LIST_SIZE && mapObj->entityList[i].pos.x != -1; i++) {
				if (mapObj->getEntityName(p) == texTable[i].name) {
					return texTable[i].texture;
				}
			}
		}
		else if (mapObj->isVisible(p, TRRN)) {
			return texTable[getTexturePos(mapObj->getTerrainName(p))].texture;
		}
		break;
	default1:
		break;
	}
	return NULL;
}

void graphics::reloadTextures() {
	for (int i = 0; i < mapObj->getTerrainListSize(); i++) {
		if (mapObj->isVisible(i)) {
			loadTexture(mapObj->getTextureName(i));
		}
	}

}