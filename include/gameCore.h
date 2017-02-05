#include "entityx/entityx.h"
#include "states/state_base.h"
#include <stack>
#include "graphicsSystem.h"

namespace ex = entityx;

class gameCore {

public:
	gameCore();
	~gameCore();
	void pushState(std::unique_ptr<State::State_Base> state);
	void popState();
	void gameLoop();

	graphicsSystem* getGraphicsObj();
private:	
	stack<std::unique_ptr<State::State_Base>> states;
	graphicsSystem* graphicsObj;
};
