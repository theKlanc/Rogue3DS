#pragma once
#include "State_Base.h"

namespace State {
	class Playing : public State_Base {
	public:
		Playing(gameCore& application);

		void input();
		void update(float dt);
		void draw();

	private:

	};
}