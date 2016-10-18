#include <string>
#include "core.h"
#include "graphicsSystem.h"
#include "sound.h"
#include "gameMap.h"
#include "entityx/entityx.h"

namespace ex = entityx;

class gameCore {
private:


	/**
	* \brief Moves currentEntity in the dir direction, if possible
	* \param currentEntity Entity that will be moved
	* \param dir Direction to move the entity
	*/
	void moveEntity(entity &currentEntity, direction dir) const;
	/**
	 * \brief Updates a specific entity
	 * \param currentEntity Entity to be updated
	 */
	void updateEntity(entity &currentEntity);
	/**
	 * \brief Updates the entire list of entities
	 */
	void updateEntities();

	/**
	 * \brief Handles player input
	 */
	void handleInput();

	/**
	 * \brief Game loop to be ran 60 times per second
	 */
	void gameLoop();

	bool exitBool;
	int kDown;
	int kUp;
	int kHeld;

	ex::EntityX* EntityWorld;
	gameMap* map;
	sound soundObj;
	long long tick;
	point3D* playerPos;
	/**
	 * \brief Creates a new save file
	 * \param saveName Name of the savefile to be created
	 */
	static void createSavefile(string saveName);
	/**
	 * \brief Loads the state from a savefile into memory
	 * \param saveName Name of the savefile to be loaded
	 */
	void loadSavefile(string saveName);
	/**
	 * \brief Starts the game using the loaded save
	 */
	void gameLaunch();
	string saveName;
public:
	/**
	 * \brief Default constructor
	 */
	gameCore();
	~gameCore();
	/**
	 * \brief Game menu
	 */
	void exit();
	void gameMenu();

	
};
