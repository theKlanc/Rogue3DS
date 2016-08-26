#include <3ds.h>
#include <sf2d.h>
#include "3ds/thread.h"
#include "gameCore.h"

#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <iomanip>

using namespace std;

int main()
{
	srvInit();
	aptInit();
	hidInit();
	ndspInit();
	sf2d_init();
	gfxSet3D(false);
	consoleInit(GFX_BOTTOM, NULL);
	sf2d_set_clear_color(RGBA8(0x00, 0x00, 0x00, 0xFF));
	srand(time(NULL));
	gameCore gameCore1;
	gameCore1.gameMenu();
	ndspExit();
	// Exit
	sf2d_fini();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}