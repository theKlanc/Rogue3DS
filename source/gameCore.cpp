#include <iostream>
#include <string>
#include <fstream>
#include "gameCore.h"
#include "core.h"

using namespace std;

/**
 * \brief Moves currentEntity in the dir direction, if possible
 * \param currentEntity Entity that will be moved
 * \param dir Direction to move the entity
 */
void gameCore::moveEntity(entity &currentEntity, direction dir) const
{
	switch (dir) {
	case DOWN:
		if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z - 1)) && map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z - 1) == 0) {
			currentEntity.pos.z--;
		}
		break;
	case UP:
		if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1)) && map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) == 0 && map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z - 1) == 1) {
			currentEntity.pos.z++;
		}
		break;
	case FRONT:
		if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x, currentEntity.pos.y - 1, currentEntity.pos.z)) && map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y - 1, currentEntity.pos.z) == 0) {
			currentEntity.pos.y--;
		}
		else if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x, currentEntity.pos.y - 1, currentEntity.pos.z + 1)) && (map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) | map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y - 1, currentEntity.pos.z + 1)) == 0) {
			currentEntity.pos.y--;
			currentEntity.pos.z++;
		}
		break;
	case BACK:
		if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x, currentEntity.pos.y + 1, currentEntity.pos.z)) && map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y + 1, currentEntity.pos.z) == 0) {
			currentEntity.pos.y++;
		}
		else if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x, currentEntity.pos.y + 1, currentEntity.pos.z + 1)) && (map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) | map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y + 1, currentEntity.pos.z + 1)) == 0) {
			currentEntity.pos.y++;
			currentEntity.pos.z++;
		}
		break;
	case LEFT:
		if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x - 1, currentEntity.pos.y, currentEntity.pos.z)) && map->simpleCollision(currentEntity.pos.x - 1, currentEntity.pos.y, currentEntity.pos.z) == 0) {
			currentEntity.pos.x--;
		}
		else if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x - 1, currentEntity.pos.y, currentEntity.pos.z + 1)) && (map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) | map->simpleCollision(currentEntity.pos.x - 1, currentEntity.pos.y, currentEntity.pos.z + 1)) == 0) {
			currentEntity.pos.x--;
			currentEntity.pos.z++;
		}
		break;
	case RIGHT:
		if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x + 1, currentEntity.pos.y, currentEntity.pos.z)) && map->simpleCollision(currentEntity.pos.x + 1, currentEntity.pos.y, currentEntity.pos.z) == 0) {
			currentEntity.pos.x++;
		}
		else if (map->isChunkLoaded(map->getChunk(currentEntity.pos.x + 1, currentEntity.pos.y, currentEntity.pos.z + 1)) && (map->simpleCollision(currentEntity.pos.x, currentEntity.pos.y, currentEntity.pos.z + 1) | map->simpleCollision(currentEntity.pos.x + 1, currentEntity.pos.y, currentEntity.pos.z + 1)) == 0) {
			currentEntity.pos.x++;
			currentEntity.pos.z++;
		}
		break;
	default:
		break;
	}
}

void gameCore::updateEntity(entity &currentEntity)
{
	point3D c = currentEntity.pos;
	c.z--;
	if (map->simpleCollision(c) == 0 && currentEntity.fly == 0) {
		//cout<< "que es faci la gravetat " << endl;
		currentEntity.pos.z--;
		if (currentEntity.pos.z < 0) {
			cout << "has caigut del mon, capoll" << endl;
		}
	}
}

void gameCore::updateEntities()
{
	for (int i = 0; i < ENTITY_LIST_SIZE && map->entityList[i].pos.x >= 0; i++)
	{
		updateEntity(map->entityList[i]);
	}
}

void gameCore::handleInput()
{
	if (kHeld & KEY_START) {
		cout << "EXITING..." << endl;
		exitBool = true;
	}
	if (!(kHeld & KEY_RIGHT))
	{
	}
	else
	{
		moveEntity(*player, RIGHT);
	}
	if (kHeld & KEY_LEFT) {
		moveEntity(*player, LEFT);
	}
	if (kHeld & KEY_UP) {
		moveEntity(*player, FRONT);
	}
	if (kHeld & KEY_DOWN) {
		moveEntity(*player, BACK);
	}
	if (kHeld & KEY_A) {
		moveEntity(*player, UP);
	}
}

void gameCore::gameLoop()
{
	hidScanInput();
	kDown = kDown | hidKeysDown();
	kHeld = kHeld | hidKeysHeld();
	kUp = kUp | hidKeysUp();
	if (tick % 12 == 0) {
		updateEntities();
		handleInput();
		graphicsObj.drawFrame();
		sf2d_swapbuffers();
		cout << player->pos.z << endl;
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		kUp = hidKeysUp();
	}
	else gspWaitForVBlank();

	//cout<< player->posX << ' ' << player->posY << ' ' << player->posZ << endl;




	//processar input
	//refrescar coses carregades
	//dibuixar


}

void gameCore::gameLaunch()
{

	// Inits

	//demanar arxius a carregar i ficar un menu o halgo

	//

	/*
	ifstream index;
	index.open("saves/index.txt");
	int saveNum;
	index >> saveNum;
	string saveList[saveNum];
	for (int i=0;i<saveNum;i++){
	index >> saveList[i];
	}
	index.close();
	//select which one
	saveName = selected;
	*/


	player = map->entityList;
	ifstream general;
	general.open("saves/" + saveName + "/general.txt");
	if (!general.is_open()) {
		cout << "couldn't open file: " << ("saves/" + saveName + "/general.txt") << endl;
	}
	string playerSprite = "player.png", playerName;
	general >> playerName >> player->pos.x >> player->pos.y >> player->pos.z;
	general.close();
	player->spriteName = playerSprite;
	for (int i = 0; i < CHUNK_NUM; i++) {
		map->loadNewChunk(player->pos);
	}
	map->loadTerrainTable();
	graphicsObj.edit(*map, *player);
	graphicsObj.reloadTextures();
	soundObj.playFromFile("data/sounds/bgm/wilderness.ogg");
	map->startChunkLoader(&player->pos);
	tick = 0;
	while (aptMainLoop() && !exitBool) {
		gameLoop();
		tick++;
	}
	
	string generalFile = "saves/" + saveName + "/general.txt";
	std::remove(generalFile.c_str());
	ofstream generalO(generalFile);
	generalO << playerName << endl << player->pos.x << endl << player->pos.y << endl << player->pos.z << endl;
	generalO.close();
	
	map->exit();
	soundObj.exit();
	graphicsObj.freeAllTextures();

	delete map;
}

void gameCore::createSavefile(string saveName)
{

	fsCreateDir("saves/" + saveName + "/");
	fsCreateDir("saves/" + saveName + "/chunks/");
	std::ifstream  iFile1("data/gameData/defaultSavefile/general.txt", std::ios::binary);
	std::ofstream  oFile1("saves/" + saveName + "/general.txt", std::ios::binary);
	oFile1 << iFile1.rdbuf();
	std::ifstream  iFile2("data/gameData/defaultSavefile/terrainTable.txt", std::ios::binary);
	std::ofstream  oFile2("saves/" + saveName + "/terrainTable.txt", std::ios::binary);
	oFile2 << iFile2.rdbuf();
	iFile1.close();
	iFile2.close();
	oFile1.close();
	oFile2.close();

}

void gameCore::loadSavefile(string saveID)
{
	map = new gameMap(saveID);
	saveName = saveID;
}

gameCore::gameCore()
{
	kDown = 0;
	kUp = 0;
	kHeld = 0;
	player = nullptr;
	map = nullptr;
	exitBool = false;
	tick = 0;
}

gameCore::~gameCore()
{
}

void gameCore::gameMenu()
{
	/*
	<Create save>
	<Load save>
	<Options>
	<Quit>
	*/
	createSavefile("default");
	loadSavefile("default");
	//int option = 0;
	//sf2d_texture*  unpressedButton = sfil_load_PNG_file("data/sprites/unpressed_button.png", SF2D_PLACE_RAM);
	//sf2d_texture*  pressedButton = sfil_load_PNG_file("data/sprites/pressed_button.png", SF2D_PLACE_RAM);

	//while (aptMainLoop()) {
	//	hidScanInput();
	//	kDown = hidKeysDown();
	//	kHeld = hidKeysHeld();
	//	kUp = hidKeysUp();

	//	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

	//	sf2d_draw_texture(unpressedButton, j * 16, i * 16);

	//	sf2d_end_frame();

	//}

	gameLaunch();
}
