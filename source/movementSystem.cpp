#include "../include/movementSystem.h"
#include "../include/components.h"
#include "../include/HardwareInterface.h"

movementSystem::~movementSystem()
{
}

void movementSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	HI::debugPrint("Movement \n");
	entityx::ComponentHandle<Position> position;
	entityx::ComponentHandle<Velocity> velocity;
	entityx::ComponentHandle<Player> player;
	for (entityx::Entity entity : es.entities_with_components(position, velocity)) {
		// Do things with entity, position and direction.
		point3D temp = position->currentPosition;
		temp.x += velocity->currentVelocity.x;
		temp.y += velocity->currentVelocity.y;
		temp.z += velocity->currentVelocity.z;
		if(!mapObj->simpleCollision(temp))
		{
			position->currentPosition = temp;
		}
		else
		{
			temp.z++;
			if(!mapObj->simpleCollision(temp))
			{
				position->currentPosition = temp;
			}
		}
		temp = position->currentPosition;
		temp.z--;
		if(!mapObj->simpleCollision(temp))
		{
			position->currentPosition = temp;
		}
		
	}
	HI::debugPrint("END \n");
}