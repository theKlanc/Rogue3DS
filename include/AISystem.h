#pragma once
#include "entityx/entityx.h"
#include "core.h"

using namespace std;
namespace ex = entityx;

class AISystem : public ex::System<AISystem>
{
public:
	explicit AISystem(){}
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
	~AISystem();
private:
};

