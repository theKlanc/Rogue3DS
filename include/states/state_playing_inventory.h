#pragma once
#include "state_base.h"
#include "../core.h"
#include "../sound.h"
#include "../gameMap.h"

namespace State {
	class PlayingInventory : public State_Base {
	public:
		PlayingInventory(gameCore& application);
		~PlayingInventory();

		void input() override;
		void update(float dt) override;
		void draw() override;
	};
}
