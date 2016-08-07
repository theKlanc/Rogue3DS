#include <3ds.h>
#include <string>
#include "core.h"
#include "graphics.h"
#include "sound.h"
#include "gameMap.h"

class gameCore {
private:


	void moveEntity(entity &currentEntity, direction dir);
	void updateEntity(entity &currentEntity);
	void updateEntities();

	void handleInput();

	void gameLoop();

	bool exitBool;
	u32 kDown;
	u32 kUp;
	u32 kHeld;

	gameMap* map;
	graphics graphicsObj;
	sound soundObj;
	entity* player;
	long long tick;
	void createSavefile(string saveName);
	void loadSavefile(string saveName);
	void gameLaunch();
	string saveName;
public:
	gameCore();
	void gameMenu();
	//aviat aixo va cap a private;

	
};
