 #pragma once
#include "gameMap.h"
#include "core.h"
#include "UI.h"
#include "entityx/entityx.h"
#include "HardwareInterface.h"

namespace ex = entityx;

class graphicsSystem : public ex::System<graphicsSystem>
{
private:
	point3D* playerPos;
	gameMap* mapObj;
	textureName texTable[TEX_TABLE_SIZE];
	bool isTextureLoaded(string textureFile) const;
	int freeTexturePos() const;
	int getTexturePos(string fileName) const;
	int loadTexture(string fileName);
	void freeTexture(string fileName);
	void cameraUpdate();
	void loadEntityTextures(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt);
	HI::HITexture getTexture(point3D p) const;
	HI::HITexture arrowTexture;
	point3D cameraPos;
public:
	explicit graphicsSystem(gameMap* map, point3D* pos);
	~graphicsSystem();

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
	void freeAllTextures();
	void drawFrame(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt);
	void reloadTextures();

};
