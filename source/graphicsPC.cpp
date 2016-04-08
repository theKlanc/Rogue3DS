#ifdef _WIN32
#include <SFML/Graphics.hpp>
#include "gameMap.h"
#include "graphicsPC.h"

using namespace std;

graphics::graphics() {
	loadTexture("player.png");
	window.create(sf::VideoMode(400, 240), "RoguePC");
}
graphics::graphics(gameMap &map, entity &playerOrig) {
	loadTexture("player.png");
	mapObj = &map;
	player = &playerOrig;
	window.create(sf::VideoMode(400, 240), "RoguePC");
}

void graphics::edit(gameMap &map, entity &playerOrig) {
	mapObj = &map;
	player = &playerOrig;
}

void graphics::drawFrame() {
	window.clear();
	//window.draw();
	sf::Sprite sprite[15][25];
	for (int i = 0; i != 15; i++) {
		for (int j = 0; j != 25; j++) {
			for (int y = RENDER_HEIGHT; y >= 0; y--) {
				point3D p;
				p.x = (player->pos.x + j) - 12;
				p.y = (player->pos.y + i) - 7;
				p.z = (player->pos.z - y);
				if (mapObj->isVisible(p, PRRT)) {
					sprite[i][j].setTexture(*getTexture(p, PRRT));
					sprite[i][j].setPosition(sf::Vector2f(j * 16, i * 16));
					window.draw(sprite[i][j]);
				}
			}
		}
	}
	window.display();
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
#ifdef _WIN32
	texTable[freeTextureLoc].texture.loadFromFile(fileName);
	texTable[freeTextureLoc].texture.setSmooth(false);
#else
	texTable[freeTextureLoc].texture = sfil_load_PNG_file(fileName.c_str(), SF2D_PLACE_RAM);
	sf2d_texture_tile32(texTable[freeTextureLoc].texture);
#endif

}
void graphics::freeTexture(string fileName) { //frees a texture from texTable[]
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
void graphics::freeAllTextures() {	 //frees all textures
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		texTable[i].name = "free";
#ifndef _WIN32
		sf2d_free_texture(texTable[i].texture);
#endif
	}
}


sf::Texture* graphics::getTexture(point3D p, mode mode_t) {

	point3D b;
	b.x = floor(p.x / CHUNK_SIZE);												   //AKESTA FUNCIO ES EL PUTO SIDA, I DEMOSTRA QUE HI HA MOLTA COSA A CANVIAR, MOLTISSIMA
	b.y = floor(p.y / CHUNK_SIZE);
	b.z = floor(p.z / CHUNK_SIZE);

	int chunkPosition = mapObj->getChunkPos(b);
	switch (mode_t) {
	case TRRN:
		if (chunkPosition == -1) {
			getTexture(p, NTT);
		}
		if (mapObj->isVisible(p, TRRN)) {
			return &texTable[getTexturePos(mapObj->getTerrainName(p))].texture;
		}
		break;
	case NTT:
		for (int i = 0; i < ENTITY_LIST_SIZE && mapObj->entityList[i].pos.x != -1; i++) {
			if (mapObj->getEntityName(p) == texTable[i].name) {
				return &texTable[i].texture;
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
					return &texTable[i].texture;
				}
			}
		}
		else if (mapObj->isVisible(p, TRRN)) {
			return &texTable[getTexturePos(mapObj->getTerrainName(p))].texture;
		}
		break;
	}
}

void graphics::reloadTextures() {
	for (int i = 0; i < mapObj->getTerrainListSize(); i++) {
		if (mapObj->isVisible(i)) {
			loadTexture(mapObj->getTextureName(i));
		}
	}
}
#endif	