#ifdef _VITA
#include "../include/core.h"
#include "../include/HardwareInterface.h"
#include <vita2d.h>

void HI::systemInit()
{
}

void HI::systemFini()
{
}

void HI::consoleInit()
{
}

void HI::consoleFini() {};

void HI::startFrame(HI_SCREEN screen)
{
	vita2d_start_drawing();
}

short HI::getRComponent(HIColor color)
{
	return (short)color;
}

short HI::getGComponent(HIColor color)
{
	return (short)color >> 8;
}
short HI::getBComponent(HIColor color)
{
	return (short)color >> 16;
}
short HI::getAComponent(HIColor color)
{
	return (short)color >> 24;
}

void HI::setBackgroundColor(HIColor color)
{
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
}

HI::HITexture HI::loadPngFile(std::string path)
{
	return (HI::HITexture)&path;
}
HI::HITexture HI::loadBmpFile(std::string path)
{
	return (HI::HITexture)&path;
}

void HI::drawTexture(HI::HITexture texture, int posX, int posY)
{
}

void HI::drawTextureRotate(HI::HITexture texture, int posX, int posY, float angle)
{
}

void HI::drawTexturePart(HI::HITexture texture, int startX, int startY, int posX, int posY, int sizeX, int sizeY)
{
}

void HI::mergeTextures(HITexture origin, HITexture destination, short posX, short posY)
{
}

void HI::drawRectangle(int posX, int posY, int width, int height, HI::HIColor color)
{
}

void HI::freeTexture(HITexture texture)
{
}
HI::HITexture HI::createTexture(int sizeX, int sizeY)
{
	vita2d_create_empty_texture(sizeX, sizeY);
}

void HI::endFrame()
{
}						  

void HI::swapBuffers()
{
}

std::string HI::getDataPath()
{
	return "data/";
}

std::string HI::getSavesPath()
{
	return "saves/";
}

bool HardwareInterface::createDir(std::string path)
{
	return false;
}
bool HardwareInterface::copyFile(std::string input, std::string output)
{
	return false;
}

int HI::getScreenHeight()
{
	return 240;
}
int HI::getScreenWidth()
{
	return 400;
}

HardwareInterface::HI_CONSOLE HI::getConsole()
{
	return CONSOLE_PSVITA;
}

void HI::createThread(void* entrypoint, void* arg, size_t stack_size, int prio, int affinity, bool detached)
{
}

void HI::updateTouch(point2D &touch)
{
}
void HI::updateHID()
{
}

int HI::getKeysUp()
{return 0;
}
int HI::getKeysHeld()
{return 0;
}
int HI::getKeysDown()
{return 0;
}

void HI::getCirclePadPos(point2D &circle, HI_CIRCLEPAD circlePadID)
{
}

void HI::sleepThread(unsigned long ns)
{
}


void* HI::linearAllocator(size_t size)
{
	return (void*)&size;
}

//Sound
void HI::dspSetOutputMode(dspOutputMode mode)
{
}
void HI::dspChnSetInterp(int id, HI::dspInterpType type)
{
}
void HI::dspChnSetFormat(int id, unsigned short format)
{
}
void HI::dspChnSetRate(int id, float rate)
{
}
void HI::dspChnSetMix(int id, float mix[12])
{
}
void HI::dspChnWaveBufAdd(int id, HI::dspWaveBuf* buf)
{
}

//??
void HI::DSP_FlushDataCache(const void* address, unsigned int size)
{
}

void HI::gspWaitForEvent(HardwareInterface::GSPGPU_Event id, bool nextEvent)
{
}

void HI::waitForVBlank()
{
}

bool HI::aptMainLoop()
{
}
#endif