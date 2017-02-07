#include "../../include/states/state_playing_pause.h"
#include "../../include/gameCore.h"

namespace State {
	PlayingPause::PlayingPause(gameCore& application)
		: State_Base(application) {
		application.getGraphicsObj()->loadTexture("rightArrow.png");
	}
	PlayingPause::~PlayingPause() {
		core->getGraphicsObj()->freeTexture("rightArrow.png");

	}


	void PlayingPause::input() {
		HI::updateHID();
		auto keys = HI::getKeysDown();
		if (keys & HI::HI_KEY_A) {
			switch (option) {
			case 0:
			{
				core->popState();
				break;
			}
			case 1:
			{
				core->popState(2);
				break;
			}
			default: {break; }
			}
		}
		else if (keys & HI::HI_KEY_DOWN) { if (option < 1)option++; }
		else if (keys & HI::HI_KEY_UP) { if (option > 0)option--; }
	}

	void PlayingPause::update(float dt) {
	}

	void PlayingPause::draw() {
		HI::startFrame(HardwareInterface::SCREEN_TOP);
		HI::HITexture arrow = core->getGraphicsObj()->getTexture("rightArrow.png");
		core->getUIObj()->drawPanel(5, 5, 128, 128, "normal");
		core->getUIObj()->drawText("Unpause", 40, 20, 10, RGBA8(0,0,0,255));
		core->getUIObj()->drawText("Exit", 40, 60, 10, RGBA8(0, 0, 0, 255));
		switch (option) {
		case 0:
		{
			HI::drawTexture(arrow, 20, 20);
			break;
		}
		case 1:
		{
			HI::drawTexture(arrow, 20, 60);
			break;
		}
		default: {break; }
		}
		HI::endFrame();
		HI::swapBuffers();
	}
}
