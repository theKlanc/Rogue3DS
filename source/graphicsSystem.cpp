#include "../include/gameMap.h"
#include "../include/graphicsSystem.h"
#include "../include/entityx/entityx.h"
#include "../include/components.h"
#include "../include/HardwareInterface.h"

using namespace std;

graphicsSystem::graphicsSystem(gameMap* map, point3D* pos): playerPos(pos), mapObj(map), cameraPos(*pos)
{
	arrowTexture = HI::loadPngFile(HI::getDataPath()+"sprites/arrow.png");
	reloadTextures();
}

graphicsSystem::~graphicsSystem()
{
	freeAllTextures();
}

void graphicsSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
		loadEntityTextures(es, events, dt);
	drawFrame(es,events,dt);
}

void graphicsSystem::drawFrame(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) {
	cameraUpdate();
	struct queueElement
	{
		point3D pos;
		unsigned short spritePos;
	};
	int queueNumber = 0;
	queueElement entityQueue[32];
	entityx::ComponentHandle<FixedSprite> fixedSprite;
	entityx::ComponentHandle<Position> position;

	for (entityx::Entity entity : es.entities_with_components(position, fixedSprite))
	{
		entityQueue[queueNumber].pos = position->currentPosition;
		entityQueue[queueNumber].spritePos = fixedSprite->texPos;
		queueNumber++;
	}
	HI::startFrame(HI::SCREEN_TOP);
	point3D p;
	for (int i = 0; i != 15; i++) {
		for (int j = 0; j != 25; j++) {
			for (int y = RENDER_HEIGHT; y >= 0; y--) {
				p.x = (cameraPos.x + j) - 12;
				p.y = (cameraPos.y + i) - 7;
				p.z = (cameraPos.z - y);
				//cout << "X " << p.x << " Y " << p.y << " Z " << p.z << endl;
				if (p.x >= 0 && p.y >= 0 && p.z >= 0 && mapObj->isVisible(p)) {
					HI::drawTexture(getTexture(p, TRRN), j * 16, i * 16);
					if (y > 1) HI::drawTextureRotate(arrowTexture, j * 16 + 8, i * 16 + 8, PI);
					else if (y == 0) HI::drawTexture(arrowTexture, j * 16, i * 16);
				}
				for(int k = 0; k<queueNumber;k++)
				{
					if(entityQueue[k].pos.x == p.x && entityQueue[k].pos.y == p.y && entityQueue[k].pos.z == p.z)
					{
						HI::drawTexture(texTable[entityQueue[k].spritePos].texture, j * 16,i * 16);
					}
				}
			}
		}
	}


	HI::endFrame();

	HI::swapBuffers();
}

bool graphicsSystem::isTextureLoaded(string textureFile) const
{ // tells if a texture with said name is present on texTable
	for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != ""; i++) {
		if (texTable[i].name == textureFile) {
			return true;
		}
	}
	return false;
}
int graphicsSystem::freeTexturePos() const
{ //returns the position inside texTable[] of the first free texture space
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		if (texTable[i].name == "") {
			return i;
		}
	}
	//cout<< "NO FREE SPACE IN TEXTABLE" << endl;
	return -1;
}
int graphicsSystem::getTexturePos(string fileName) const
{ //returns the position inside texTable[] of the texture with said filename
	for (int i = 0; i < TEX_TABLE_SIZE && texTable[i].name != ""; i++) {
		if (texTable[i].name == fileName) {
			return i;
		}
	}
	//cout<< "NO TEXTURE W/ FNAME " << fileName << " FOUND" << endl;
	return -1;
}
int graphicsSystem::loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
	int freeTextureLoc = freeTexturePos();
	texTable[freeTextureLoc].name = fileName;
	fileName = HI::getDataPath()+"sprites/" + fileName;
	texTable[freeTextureLoc].texture = HI::loadPngFile(fileName.c_str());
	return freeTextureLoc;
}
void graphicsSystem::freeTexture(string fileName) { //frees a texture from texTable[]
	int textureLocation = getTexturePos(fileName);
	int freeTexLoc = freeTexturePos();
	textureName temp = texTable[textureLocation];
	texTable[textureLocation] = texTable[freeTexLoc - 1];
	texTable[freeTexLoc - 1] = temp;
	texTable[freeTexLoc - 1].name = "";

	HI::freeTexture(texTable[freeTexLoc - 1].texture);

}
void graphicsSystem::freeAllTextures() {	 //frees all textures
	for (int i = 0; i < TEX_TABLE_SIZE; i++) {
		if (texTable[i].name != "") {
			texTable[i].name = "";
			HI::freeTexture(texTable[i].texture);
		}
	}
}


void graphicsSystem::cameraUpdate()
{
	if (cameraPos.x - 5 < playerPos->x) { cameraPos.x++; }
	if (cameraPos.x + 4 > playerPos->x) { cameraPos.x--; }
	if (cameraPos.y - 4 < playerPos->y) { cameraPos.y++; }
	if (cameraPos.y + 3 > playerPos->y) { cameraPos.y--; }
	cameraPos.z =playerPos->z;
}

void graphicsSystem::loadEntityTextures(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	entityx::ComponentHandle<FixedSprite> FixedSprite;
	for (entityx::Entity entity : es.entities_with_components(FixedSprite))
	{
		if(!FixedSprite->isLoaded)
		{
			FixedSprite->isLoaded = true;
			FixedSprite->texPos = loadTexture(FixedSprite->filename);
		}
	}
}

HI::HITexture graphicsSystem::getTexture(point3D p, mode mode_t) const
{

	point3D b;
	b.x = floor(p.x / CHUNK_SIZE);
	b.y = floor(p.y / CHUNK_SIZE);
	b.z = floor(p.z / CHUNK_SIZE);

	int chunkPosition = mapObj->getChunkID(b);
	switch (mode_t) {
	case TRRN:
		if (chunkPosition == -1) {
			getTexture(p, NTT);
		}
		if (mapObj->isVisible(p)) {
			return texTable[getTexturePos(mapObj->getTerrainName(p))].texture;
		}
		break;
	default:
		break;
	}
	return nullptr;
}

void graphicsSystem::reloadTextures() {
	for (int i = 0; i < mapObj->getTerrainListSize(); i++) {
		if (mapObj->isVisible(i)) {
			loadTexture(mapObj->getTextureName(i));
		}
	}
}