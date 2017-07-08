#include "../include/movementSystem.h"
#include "../include/components.h"
#include "../include/HardwareInterface.h"

movementSystem::~movementSystem() {
}

void movementSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) {
	entityx::ComponentHandle<Position> position;
	entityx::ComponentHandle<Velocity> velocity;
	entityx::ComponentHandle<Jump> jump;
	entityx::ComponentHandle<canFloat>cFloat;
	entityx::ComponentHandle<autoFloat>aFloat;	  
	for (entityx::Entity entity : es.entities_with_components(position, velocity, jump)) {	 //Jumping
		point3D temp = position->currentPosition;
		temp.z--;
		if (jump->jumping && mapObj->simpleCollision(temp)) {
			velocity->currentVelocity.z = 2;
		}
	}
	for (entityx::Entity entity : es.entities_with_components(position, velocity, cFloat)) {   //Floating
		point3D temp = position->currentPosition;
		temp.z--;
		if (cFloat->floating && (mapObj->canFloat(position->currentPosition))) {
			if (mapObj->canFloat(point3D(position->currentPosition.x, position->currentPosition.y, position->currentPosition.z + 1))) {
				velocity->currentVelocity.z = 2;
			}
			else {
				velocity->currentVelocity.z = 1;
			}
		}
	}
	for (entityx::Entity entity : es.entities_with_components(position, velocity, aFloat)) {   //AutoFloating
		point3D temp = position->currentPosition;
		temp.z--;
		if (mapObj->canFloat(position->currentPosition)) {
			if (mapObj->canFloat(point3D(position->currentPosition.x, position->currentPosition.y, position->currentPosition.z + 1))) {
				velocity->currentVelocity.z = 2;
			}
			else {
				velocity->currentVelocity.z = 1;
			}
		}
	}
	for (entityx::Entity entity : es.entities_with_components(position, velocity)) {         //physics
		// Do things with entity, position and direction.
		point3D temp = position->currentPosition;
		temp.x += velocity->currentVelocity.x;
		temp.y += velocity->currentVelocity.y;
		temp.z += velocity->currentVelocity.z;
		if (!mapObj->simpleCollision(temp))		//Si la posicio on estara no esta ocupada
		{
			position->currentPosition = temp;			//el posem alla on estara
		}
		else										  //Sino intentem k salti o hi floti
		{
			temp.z++;
			point3D testPos = position->currentPosition;
			testPos.z--;
			if (!mapObj->simpleCollision(temp) && (mapObj->simpleCollision(testPos) || mapObj->canFloat(position->currentPosition))) {
				position->currentPosition = temp;
			}
		}
		temp = position->currentPosition;
		temp.z--;
		if (!mapObj->simpleCollision(temp)) {
			position->currentPosition = temp;
		}
		velocity->currentVelocity = point3D(0, 0, 0);

	}
}
