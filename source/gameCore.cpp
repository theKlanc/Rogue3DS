#include "../include/entityx/entityx.h"
#include "../include/gameCore.h"
#include "../include/states/state_playing.h"
#include "../include/states/state_main_menu.h"


using namespace std;
namespace ex = entityx;

void gameCore::gameLoop() {
	while (HI::aptMainLoop() && !states.empty() && !exit) {
		states.top()->input();
		states.top()->update(0);
		states.top()->draw();
	}
}

void gameCore::quit()
{
	exit = true;
}

graphics* gameCore::getGraphicsObj() const
{
	return graphicsObj;
}

UI* gameCore::getUIObj() const
{
	return uiObj;
}

gameCore::gameCore() {
	pushState(make_unique<State::MainMenu>(*this));
	graphicsObj = new graphics;
	exit = false;
	uiObj = new UI(graphicsObj);
}

gameCore::~gameCore() {
	while (!states.empty()) states.pop();
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	states.push(std::move(state));
}

void gameCore::popState(int n) {
	for(int i = 0;i<n;i++)states.pop();
}
