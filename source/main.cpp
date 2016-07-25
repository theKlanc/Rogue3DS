#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include "graphics.h"
#include "sound.h"
#include "3ds/thread.h"
#include "gameCore.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include "core.h"
#include "gameMap.h"
#include <dirent.h>

using namespace std;

int main()
{

	srvInit();
	aptInit();
	hidInit();
	ndspInit();
	sf2d_init();
	consoleInit(GFX_BOTTOM, NULL);
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
	srand(time(NULL));
	
	gameCore gameCore1;
	gameCore1.gameMenu();
	ndspExit();
	// Exit
	//map.freeAllTextures();
	sf2d_fini();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}