#pragma once
#include "state_base.h"
#include "../core.h"
#include "../sound.h"
#include "../gameMap.h"

namespace State {
	class Playing : public State_Base {
	public:
		Playing(gameCore& application, string save);
		~Playing();

		void input() override;
		void update(float dt) override;
		void draw() override;

	private:
		void cameraUpdate(point3D pos);
		void loadTerrainTextures() const;

		gameMap* map;
		sound soundObj;
		long long tick;
		point3D* playerPos;	 //treure ho
		string saveName;
		entityx::EntityX* EntityWorld;
		entityx::Entity* playerEntity;
		string playerName; //treure ho
		point3D cameraPos;
	};
}
