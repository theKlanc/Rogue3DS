#include "../include/entityx/entityx.h"
#include "../include/gameCore.h"
#include "../include/states/state_playing.h"
#include "../include/states/state_main_menu.h"


using namespace std;
namespace ex = entityx;

void gameCore::gameLoop() {
	while (HI::aptMainLoop()) {
		states.top()->input();
		states.top()->update(0);
		states.top()->draw();
	}
}

graphicsSystem* gameCore::getGraphicsObj() {
	return graphicsObj;
}

gameCore::gameCore() {
	pushState(make_unique<State::MainMenu>(*this));
	graphicsObj = new graphicsSystem;
}

gameCore::~gameCore() {
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	states.push(std::move(state));
}

void gameCore::popState() {
	states.pop();
}
