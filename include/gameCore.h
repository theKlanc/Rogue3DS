#include "entityx/entityx.h"
#include "states/state_base.h"
#include <stack>
#include "graphics.h"
#include "UI.h"

namespace ex = entityx;

class gameCore {

public:
	gameCore();
	~gameCore();
	void pushState(std::unique_ptr<State::State_Base> state);
	void popState(int n = 1);
	void gameLoop();
	void quit();
	graphics* getGraphicsObj() const;
	UI* getUIObj() const;
private:	
	stack<std::unique_ptr<State::State_Base>> states;
	graphics* graphicsObj;
	UI* uiObj;
	bool exit;
};
