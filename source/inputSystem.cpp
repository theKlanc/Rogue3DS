#include "../include/inputSystem.h"
#include "../include/components.h"
#include "../include/entityx/entityx.h"
#include "../include/HardwareInterface.h"


void inputSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	HI::debugPrint("Input \n");
	entityx::ComponentHandle<Velocity> velocity;
	entityx::ComponentHandle<Player> player;
	for (entityx::Entity entity : es.entities_with_components(player,velocity)){
		// Do things with entity, position and direction.
		velocity->currentVelocity.y = 0;
		velocity->currentVelocity.x = 0;

		if (*kHeld & HI::HI_KEY_DOWN)
		{
			velocity->currentVelocity.y=1;
		}
		if (*kHeld &  HI::HI_KEY_UP)
		{
			velocity->currentVelocity.y=-1;
		}
		if (*kHeld &  HI::HI_KEY_LEFT)
		{
			velocity->currentVelocity.x=-1;
		}
		if (*kHeld &  HI::HI_KEY_RIGHT)
		{
			velocity->currentVelocity.x=1;
		}
	}
	HI::debugPrint("END \n");
}

inputSystem::~inputSystem()
{
}
