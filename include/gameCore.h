#include "entityx/entityx.h"
#include "states/state_base.h"
#include <stack>
#include "graphics.h"

namespace ex = entityx;

class gameCore {

public:
	gameCore();
	~gameCore();
	void pushState(std::unique_ptr<State::State_Base> state);
	void popState();
	void gameLoop();

	graphics* getGraphicsObj();
private:	
	stack<std::unique_ptr<State::State_Base>> states;
	graphics* graphicsObj;
};
