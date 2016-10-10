#include "../include/gameCore.h"
#include "../include/HardwareInterface.h"
#include <time.h>

using namespace std;

int main()
{
	HI::systemInit();
	//HI::consoleInit();
	HI::setBackgroundColor(RGBA8(0x00, 0x00, 0x00, 0xFF));
	srand(time(nullptr));
	gameCore gameCore1;
	gameCore1.gameMenu();
	gameCore1.exit();
	HI::systemFini();
	return 0;
}