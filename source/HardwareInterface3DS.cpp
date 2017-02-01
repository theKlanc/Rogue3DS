#ifdef _3DS
#include <iostream>
#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include "../include/core.h"
#include "../include/HardwareInterface.h"
#include <dirent.h>
#include <functional>

#define DEBUG_PRIORITY 3

void HI::systemInit() {
	srvInit();
	aptInit();
	hidInit();
	ndspInit();
	sf2d_init();
	sftd_init();
	//HI::consoleInit();
}

void HI::systemFini() {
	ndspExit();
	sftd_fini();
	sf2d_fini();
	hidExit();
	aptExit();
	srvExit();
}

void HI::consoleInit() {
	::consoleInit(GFX_BOTTOM, nullptr);
}

void HI::consoleFini() {};

void HI::startFrame(HI_SCREEN screen) {
	sf2d_start_frame(
		(screen == SCREEN_TOP ? GFX_TOP : GFX_BOTTOM),
		GFX_LEFT);
}

short HI::getRComponent(HIColor color) {
	return (short)color;
}

short HI::getGComponent(HIColor color) {
	return (short)color >> 8;
}
short HI::getBComponent(HIColor color) {
	return (short)color >> 16;
}
short HI::getAComponent(HIColor color) {
	return (short)color >> 24;
}

void HI::setBackgroundColor(HIColor color) {
	sf2d_set_clear_color(color);
}

HI::HITexture HI::loadPngFile(std::string path) {
	return sfil_load_PNG_file(path.c_str(), SF2D_PLACE_RAM);
}
HI::HITexture HI::loadBmpFile(std::string path) {
	return sfil_load_BMP_file(path.c_str(), SF2D_PLACE_RAM);
}

void HI::drawTexture(HI::HITexture texture, int posX, int posY) {
	if (texture != nullptr) sf2d_draw_texture((sf2d_texture*)texture, posX, posY);
}

void HI::drawTextureRotate(HI::HITexture texture, int posX, int posY, float angle) {
	if (texture != nullptr) sf2d_draw_texture_rotate((sf2d_texture*)texture, posX, posY, angle);
}

void HI::drawTexturePart(HI::HITexture texture, int startX, int startY, int posX, int posY, int sizeX, int sizeY) {
	if (texture != nullptr) sf2d_draw_texture_part((sf2d_texture*)texture, posX, posY, startX, startY, sizeX, sizeY);
}

void HI::mergeTextures(HITexture origin, HITexture destination, short posX, short posY)			   ////////AIXO NO FUNCA K B I NO SE NI VEIG PK FIRE
{
	sf2d_texture *orig = (sf2d_texture*)origin;
	sf2d_texture *dest = (sf2d_texture*)destination;
	C3D_SafeTextureCopy((u32*)orig->tex.data, 16, (u32*)dest->tex.data, 16, orig->tex.size, 0);

}

void HI::drawRectangle(int posX, int posY, int width, int height, HI::HIColor color) {
	sf2d_draw_rectangle(posX, posY, width, height, color);
}

void HI::freeTexture(HITexture texture) {
	sf2d_free_texture((sf2d_texture*)texture);
}
HI::HITexture HI::createTexture(int sizeX, int sizeY) {
	return (HI::HITexture)sf2d_create_texture(sizeX, sizeY, TEXFMT_RGBA8, SF2D_PLACE_RAM);
}

void HI::endFrame() {
	sf2d_end_frame();
}

void HI::swapBuffers() {
	sf2d_swapbuffers();
}

std::string HI::getDataPath() {
	return "data/";
}

std::string HI::getSavesPath() {
	return "saves/";
}

bool HardwareInterface::createDir(std::string path) {
	if (HI::fsExists(path)) {
		errno = EEXIST;
		return false;
	}
	return (mkdir(path.c_str(), 0777) == 0);
}

bool HardwareInterface::fsExists(std::string path) {
	FILE* fd = fopen(path.c_str(), "r");
	if (fd) {
		fclose(fd);
		return true;
	}
	return fsIsDirectory(path);
}
bool HI::fsIsDirectory(const std::string path) {
	DIR* dir = opendir(path.c_str());
	if (dir) {
		closedir(dir);
		return true;
	}
	return false;
}

bool HardwareInterface::copyFile(std::string origin, std::string destination) {
	std::ifstream  iFile(origin, std::ios::binary);
	std::ofstream  oFile(destination, std::ios::binary);
	oFile << iFile.rdbuf();
	oFile.close();
	iFile.close();
}

int HI::getScreenHeight() {
	return 240;
}
int HI::getScreenWidth() {
	return 400;
}

HardwareInterface::HI_PLATFORM HI::getPlatform() {
	return PLATFORM_NINTENDO3DS;
}

void HI::createThread(void* entrypoint, std::reference_wrapper<void(void*)>entrypoint2,void* arg, size_t stack_size, int prio, int affinity, bool detached, size_t arg_size) {
	//threadCreate((ThreadFunc)entrypoint, arg, stack_size, prio, affinity, detached);
	threadCreate((ThreadFunc)(&(entrypoint2.get())), arg, stack_size, prio, affinity, detached);
}

void HI::updateTouch(point2D &touch) {
	touchPosition tPos;
	hidTouchRead(&tPos);
	touch.x = tPos.px;
	touch.y = tPos.py;
}
void HI::updateHID() {
	hidScanInput();
}

int HI::getKeysUp() {
	return hidKeysUp();
}
int HI::getKeysHeld() {
	return hidKeysHeld();
}
int HI::getKeysDown() {
	return hidKeysDown();
}

void HI::getCirclePadPos(point2D &circle, HI_CIRCLEPAD circlePadID) {
	circlePosition pos;
	hidCircleRead(&pos);
	circle.x = pos.dx;
	circle.y = pos.dy;
}

void HI::sleepThread(unsigned long ns) {
	svcSleepThread(ns);
}


void* HI::linearAllocator(size_t size) {
	return linearAlloc(size);
}

//Sound
void HI::dspSetOutputMode(dspOutputMode mode) {
	ndspSetOutputMode((ndspOutputMode)mode);
}
void HI::dspChnSetInterp(int id, HI::dspInterpType type) {
	ndspChnSetInterp(id, (ndspInterpType)type);
}
void HI::dspChnSetFormat(int id, unsigned short format) {
	ndspChnSetFormat(id, format);
}
void HI::dspChnSetRate(int id, float rate) {
	ndspChnSetRate(id, rate);
}
void HI::dspChnSetMix(int id, float mix[12]) {
	ndspChnSetMix(id, mix);
}
void HI::dspChnWaveBufAdd(int id, HI::dspWaveBuf* buf) {
	ndspChnWaveBufAdd(id, (ndspWaveBuf*)buf);
}

//??
void HI::DSP_FlushDataCache(const void* address, unsigned int size) {
	::DSP_FlushDataCache(address, size);
}

void HardwareInterface::debugPrint(string s) {
	cout << s;
}

void HardwareInterface::debugPrint(string s, int priority) {
	if (priority >= DEBUG_PRIORITY) cout << s;
}

void HI::gspWaitForEvent(HardwareInterface::GSPGPU_Event id, bool nextEvent) {
	::gspWaitForEvent((::GSPGPU_Event)id, nextEvent);
}

void HI::waitForVBlank() {
	HI::gspWaitForEvent(GSPGPU_EVENT_VBlank0, true);
}

bool HI::aptMainLoop() {
	return ::aptMainLoop();
}
#endif