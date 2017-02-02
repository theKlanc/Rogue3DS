#pragma once
#include "../core.h"

class gameCore;

namespace State {
	class State_Base {
	public:
		State_Base(gameCore& core) : kDown(0),kUp(0),kHeld(0), core(&core){}
		virtual ~State_Base() {}

		virtual void input() = 0;
		virtual void update(float dt) = 0;
		virtual void draw() = 0;

	protected: //aqui puc posar variables que els estats hagin de compartir

		gameCore* core;

		int kDown;
		int kUp;
		int kHeld;
		point2D touch;
	};
}