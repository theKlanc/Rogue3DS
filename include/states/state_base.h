#pragma once
#include "../gameCore.h"

namespace State {
	class State_Base {
	public:
		State_Base(gameCore& core);
		virtual ~State_Base() {}

		virtual void input() = 0;
		virtual void update(float dt) = 0;
		virtual void draw() = 0;

	protected: //aqui puc posar variables que els estats hagin de compartir

		gameCore *core;

	};
}