#pragma once
#include "core.h"
#include "gameMap.h"

#include "entityx/entityx.h"

struct point3D;
namespace ex = entityx;

class movementSystem : public ex::System<movementSystem>
{
public:
	explicit movementSystem(gameMap *mapObj) : mapObj(mapObj) {}
	~movementSystem();
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)override;
private:
	bool basicCollision(point3D pos);

	gameMap *mapObj;

};

