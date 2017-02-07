#pragma once
#include "entityx/entityx.h"

using namespace std;
namespace ex = entityx;

class inputSystem
{
public:
	inputSystem(){}
	static void update(entityx::EntityX* e, int kDown, int kUp, int kHeld);
	~inputSystem();

};

