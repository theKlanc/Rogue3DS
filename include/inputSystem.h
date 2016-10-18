#pragma once
#include "entityx/entityx.h"
#include "core.h"

using namespace std;
namespace ex = entityx;

class inputSystem : public ex::System<inputSystem>
{
public:
	explicit inputSystem(int* kDown, int* kUp, int* kHeld) : kDown(kDown), kUp(kUp), kHeld(kHeld){}
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
	~inputSystem();
private:
	int* kDown;
	int* kUp;
	int* kHeld;
};

