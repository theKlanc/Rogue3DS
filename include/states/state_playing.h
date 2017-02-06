#pragma once
#include "State_Base.h"
#include "../core.h"
#include "../sound.h"
#include "../gameMap.h"

namespace State {
	class Playing : public State_Base {
	public:
		Playing(gameCore& application);
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
