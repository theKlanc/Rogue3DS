#include "../../include/states/state_playing.h"
#include "../../include/gameCore.h"
#include "../../include/inputSystem.h"
#include "../../include/movementSystem.h"
#include "../../include/AISystem.h"
#include "../../include/components.h"
#include "../../include/graphicsSystem.h"

namespace State {
	Playing::Playing(gameCore& application) : State_Base(application) {
		core->getGraphicsObj()->loadTexture("arrow.png");
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
		cameraPos = *playerPos;
		EntityWorld->systems.add<inputSystem>(&kDown, &kUp, &kHeld);
		EntityWorld->systems.add<movementSystem>(map);
		EntityWorld->systems.add<AISystem>();
		EntityWorld->systems.configure();
		entityx::Entity player = EntityWorld->entities.create();
		playerEntity = &player;


		entityx::ComponentHandle<Position> position = playerEntity->component<Position>();
		map->updatePlayerPos(*playerPos);
		player.assign<Position>(*playerPos);
		for (int i = 0; i < CHUNK_NUM; i++) {
			map->loadNewChunk();
		}
		map->loadTerrainTable();
		loadTerrainTextures();
		point3D caca;
		caca.x++;
		caca.y++;
		caca.z++;
		player.assign<Velocity>(caca);
		player.assign<Player>();
		core->getGraphicsObj()->loadTexture("player.png");
		player.assign<FixedSprite>("player.png");
		//soundObj.playFromFile(HI::getDataPath()+"sounds/bgm/wilderness.ogg");
		map->startChunkLoader();
	}

	Playing::~Playing() {
		string generalFile = HI::getSavesPath() + saveName + "/general.txt";
		std::remove(generalFile.c_str());

		ofstream generalO(generalFile);
		generalO << playerName << endl << playerPos->x << endl << playerPos->y << endl << playerPos->z << endl;	//arreglar, k ara ho trenco
		generalO.close();
		map->exit();
	}

	void Playing::input() {
		HI::updateHID();
		kDown = kDown | HI::getKeysDown();
		kHeld = kHeld | HI::getKeysHeld();
		kUp = kUp | HI::getKeysUp();
		if (tick % 12 == 0) {
			kDown = HI::getKeysDown();
			kHeld = HI::getKeysHeld();
			kUp = HI::getKeysUp();
		}
		if (kHeld & HI::HI_KEY_START) {
			core->popState();
		}
	}

	void Playing::update(float dt) {
		if (tick % 12 == 0) {
			EntityWorld->systems.update_all(0);
		}
		else  HI::waitForVBlank();
		tick++;
		entityx::ComponentHandle<Position> position;
		entityx::ComponentHandle<Player> player;
		point3D pos;
		for (entityx::Entity entity : EntityWorld->entities.entities_with_components(position, player)) {
			pos = entity.component<Position>()->currentPosition;
		}
		map->updatePlayerPos(pos);
	}

	void Playing::draw() {
		cameraUpdate();
		struct queueElement {
			point3D pos;
			string spriteName;
		};
		int queueNumber = 0;
		queueElement entityQueue[32];
		entityx::ComponentHandle<FixedSprite> fixedSprite;
		entityx::ComponentHandle<Position> position;

		for (entityx::Entity entity : EntityWorld->entities.entities_with_components(position, fixedSprite)) {
			entityQueue[queueNumber].pos = position->currentPosition;
			entityQueue[queueNumber].spriteName = fixedSprite->filename;
			queueNumber++;
		}
		HI::startFrame(HI::SCREEN_TOP);
		point3D p;
		for (int i = 0; i != HI::getScreenHeight() / 16; i++) {
			for (int j = 0; j != HI::getScreenWidth() / 16; j++) {
				for (int y = RENDER_HEIGHT; y >= 0; y--) {
					p.x = (cameraPos.x + j) - (((HI::getScreenWidth() / 16) / 2) - 1);
					p.y = (cameraPos.y + i) - (((HI::getScreenHeight() / 16) / 2) - 1);
					p.z = (cameraPos.z - y);
					if (p.x >= 0 && p.y >= 0 && p.z >= 0 && map->isVisible(p)) {
						//HI::debugPrint("	drawing " + mapObj->getTerrainName(p) + "\n");
						HI::drawTexture(core->getGraphicsObj()->getTexture(map->getTerrain(p).textureFile), j * 16, i * 16);
						if (y > 1) HI::drawTextureRotate(core->getGraphicsObj()->getTexture("arrow.png"), j * 16 + 8, i * 16 + 8, PI);
						else if (y == 0) HI::drawTexture(core->getGraphicsObj()->getTexture("arrow.png"), j * 16, i * 16);
					}
					for (int k = 0; k < queueNumber; k++) {
						if (entityQueue[k].pos.x == p.x && entityQueue[k].pos.y == p.y && entityQueue[k].pos.z == p.z) {
							//HI::debugPrint("	drawing " + texTable[entityQueue[k].spritePos].name + "\n");
							HI::drawTexture(core->getGraphicsObj()->getTexture(entityQueue[k].spriteName), j * 16, i * 16);
						}
					}
				}
			}
		}


		HI::endFrame();
		HI::swapBuffers();
	}

	void Playing::cameraUpdate() {
		if (cameraPos.x - ((HI::getScreenWidth() / 16) / 5) < playerPos->x) { cameraPos.x++; }
		if (cameraPos.x + (((HI::getScreenWidth() / 16) / 5) + 1) > playerPos->x) { cameraPos.x--; }
		if (cameraPos.y - ((HI::getScreenHeight() / 16) / 5) < playerPos->y) { cameraPos.y++; }
		if (cameraPos.y + (((HI::getScreenHeight() / 16) / 5) + 1) > playerPos->y) { cameraPos.y--; }
		cameraPos.z = playerPos->z;
	}

	void Playing::loadTerrainTextures() const {
		for (int i = 0; i < map->getTerrainListSize(); i++) {
			if (map->getTerrainList()[i].visible) {
				core->getGraphicsObj()->loadTexture(map->getTerrainList()[i].textureFile);
			}
		}
	}
}
