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
		clean();
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

void gameCore::clean()
{
	while(pop>0)
	{
		states.pop();
		pop--;
	}
}

gameCore::gameCore() {

	graphicsObj = new graphics;
	exit = false;
	uiObj = new UI(graphicsObj);
	pop = 0;
	pushState(make_unique<State::MainMenu>(*this));
}

gameCore::~gameCore() {
	while (!states.empty()) states.pop();
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	states.push(std::move(state));
}

void gameCore::popState(int n) {
	pop += n;
}
