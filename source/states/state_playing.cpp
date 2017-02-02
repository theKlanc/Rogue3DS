#include "../../include/states/state_playing.h"
#include "../../include/gameCore.h"
#include "../../include/inputSystem.h"
#include "../../include/movementSystem.h"
#include "../../include/AISystem.h"
#include "../../include/components.h"
#include "../../include/graphicsSystem.h"

namespace State {
	Playing::Playing(gameCore& application): State_Base(application)
	{
		saveName = "default";
		map = new gameMap(saveName);
		EntityWorld = new ex::EntityX();
		playerPos = new point3D;
		tick = 0;

		HI::setBackgroundColor(RGBA8(0, 0, 0, 0));
		ifstream general;
		general.open(HI::getSavesPath() + saveName + "/general.txt");
		if (!general.is_open()) {
			cout << "couldn't open file: " << (HI::getSavesPath() + saveName + "/general.txt") << endl;
		}
		string playerSprite = "player.png", playerName;
		string cacota;
		general >> cacota >> playerPos->x >> playerPos->y >> playerPos->z;
		general.close();
		playerPos->z = 1 + FLOOR_HEIGHT + 100 * (1 + map->noiseObj.GetNoise(playerPos->x, playerPos->y) / 2);
		map->addPlayer(playerPos);
		for (int i = 0; i < CHUNK_NUM; i++) {
			map->loadNewChunk();
		}
		map->loadTerrainTable();
		EntityWorld->systems.add<inputSystem>(&kDown, &kUp, &kHeld);
		EntityWorld->systems.add<movementSystem>(map);
		EntityWorld->systems.add<graphicsSystem>(map, playerPos);
		EntityWorld->systems.add<AISystem>();
		EntityWorld->systems.configure();
		entityx::Entity player = EntityWorld->entities.create();
		player.assign<Position>(*playerPos);
		point3D caca;
		caca.x++;
		caca.y++;
		caca.z++;
		player.assign<Velocity>(caca);
		player.assign<Player>(playerPos);
		player.assign<FixedSprite>("player.png");
		point3D dogPos = *playerPos;
		dogPos.x += 4;
		dogPos.y += 6;
		dogPos.z = 1 + FLOOR_HEIGHT + 100 * (1 + map->noiseObj.GetNoise(dogPos.x, dogPos.y) / 2);

		entityx::Entity doggo = EntityWorld->entities.create();
		doggo.assign<AIFollower>(playerPos, 3);
		doggo.assign<Velocity>(caca);
		doggo.assign<Position>(dogPos);
		doggo.assign<FixedSprite>("doggo.png");
		//soundObj.playFromFile(HI::getDataPath()+"sounds/bgm/wilderness.ogg");
		map->startChunkLoader();	
	}

	Playing::~Playing()
	{
		string generalFile = HI::getSavesPath() + saveName + "/general.txt";
		std::remove(generalFile.c_str());

		ofstream generalO(generalFile);
		generalO << playerName << endl << playerPos->x << endl << playerPos->y << endl << playerPos->z << endl;
		generalO.close();
		map->exit();
	}

	void Playing::input() {

	}

	void Playing::update(float dt) {
		HI::updateHID();
		kDown = kDown | HI::getKeysDown();
		kHeld = kHeld | HI::getKeysHeld();
		kUp = kUp | HI::getKeysUp();
		if (tick % 12 == 0) {
			EntityWorld->systems.update_all(0);
			kDown = HI::getKeysDown();
			kHeld = HI::getKeysHeld();
			kUp = HI::getKeysUp();
		}
		else  HI::waitForVBlank();
		if (kDown & HI::HI_KEY_START) {
			core->popState();
		}
		tick++;
	}

	void Playing::draw() {

	}

}
