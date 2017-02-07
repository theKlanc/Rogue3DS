#pragma once
#include "state_base.h"
#include "../core.h"
#include "../sound.h"
#include "../gameMap.h"

namespace State {
	class PlayingPause : public State_Base {
	public:
		PlayingPause(gameCore& application);
		~PlayingPause();

		void input() override;
		void update(float dt) override;
		void draw() override;

	private:
		int option = 0;
	};
}
