#include "../include/inputSystem.h"
#include "../include/components.h"
#include "../include/entityx/entityx.h"
#include "../include/HardwareInterface.h"


void inputSystem::update(entityx::EntityX* world, int kDown, int kUp, int kHeld) {
	entityx::ComponentHandle<Velocity> velocity;
	entityx::ComponentHandle<Player> player;
	entityx::ComponentHandle<Jump> jump;
	entityx::ComponentHandle<canFloat> cFloat;
	for (entityx::Entity entity : world->entities.entities_with_components(velocity, player,jump,cFloat)) {
		velocity->currentVelocity.y = 0;
		velocity->currentVelocity.x = 0;
		if (kHeld & HI::HI_KEY_DOWN) {
			velocity->currentVelocity.y = 1;
		}
		if (kHeld &  HI::HI_KEY_UP) {
			velocity->currentVelocity.y = -1;
		}
		if (kHeld &  HI::HI_KEY_LEFT) {
			velocity->currentVelocity.x = -1;
		}
		if (kHeld &  HI::HI_KEY_RIGHT) {
			velocity->currentVelocity.x = 1;
		}
		if (kDown &  HI::HI_KEY_A) {
			jump->jumping = true;
		}
		else { jump->jumping = false; }
		if (kHeld &  HI::HI_KEY_A) {
			cFloat->floating = true;
		}
		else { cFloat->floating = false; }
	}
}

inputSystem::~inputSystem() {
}
