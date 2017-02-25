#include <future>
#ifdef WIN32
#include <iostream>
#include "../include/core.h"
#include "../include/HardwareInterface.h"	
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <functional>

#define DEBUG_PRIORITY 0

sf::RenderWindow* window;
sf::Event* event;

void HI::systemInit() {
	event = new sf::Event;
	window = new sf::RenderWindow(sf::VideoMode(1000, 1000), "Rogue3DS");
	//HI::consoleInit();
}

void HI::systemFini() {
}

void HI::consoleInit() {
}

void HI::consoleFini() {};

void HI::startFrame(HI_SCREEN screen) {
	window->clear();
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
}

HardwareInterface::HIFont HardwareInterface::loadFont(std::string path) {
	sf::Font* font = new sf::Font;
	font->loadFromFile(path);
	return font;
}

void HardwareInterface::freeFont(HIFont font) {
	delete (sf::Font*)font;
}

void HardwareInterface::drawText(HIFont font, string text, int posX, int posY, int size, HIColor color)
{
	if (font != nullptr) {
		sf::Text sftext;
		sftext.setString(text.c_str());
		sftext.setPosition(posX, posY);
		sftext.setFont(*(sf::Font*)font);
		sftext.setCharacterSize(size);
		sftext.setColor(sf::Color::Black);
		window->draw(sftext);
	}
}

HI::HITexture HI::loadPngFile(std::string path) {
	return loadBmpFile(path);
}
HI::HITexture HI::loadBmpFile(std::string path) {
	sf::Texture* texture = new sf::Texture;
	texture->loadFromFile(path);
	return texture;
}

void HI::drawTexture(HI::HITexture texture, int posX, int posY) {
	if (texture != nullptr) {
		sf::Sprite sprite;
		sprite.setTexture(*(sf::Texture*)texture);
		sprite.setPosition(sf::Vector2f(posX, posY));
		window->draw(sprite);
	}
}

void HI::drawTextureRotate(HI::HITexture texture, int posX, int posY, float angle) {
	if (texture != nullptr) {
		sf::Sprite sprite;
		sprite.setOrigin(8, 8);
		sprite.setRotation(360 * angle / (2 * PI));
		sprite.setTexture(*(sf::Texture*)texture);
		sprite.setPosition(sf::Vector2f(posX, posY));
		window->draw(sprite);
	}
}

void HI::drawTexturePart(HI::HITexture texture, int startX, int startY, int posX, int posY, int sizeX, int sizeY) {
	if (texture != nullptr) {
		sf::Sprite sprite;
		sprite.setTexture(*(sf::Texture*)texture);
		sprite.setPosition(sf::Vector2f(posX, posY));
		sprite.setTextureRect(sf::IntRect(startX, startY, sizeX, sizeY));
		window->draw(sprite);
	}
}

void HI::mergeTextures(HITexture origin, HITexture destination, short posX, short posY)			   ////////AIXO NO FUNCA K B I NO SE NI VEIG PK FIRE
{
}

void HI::drawRectangle(int posX, int posY, int width, int height, HI::HIColor color) {
	sf::RectangleShape rectangle(sf::Vector2f(width, height));
	rectangle.setFillColor(sf::Color(color));
	window->draw(rectangle);
}

void HI::freeTexture(HITexture texture) {
	texture = &sf::Texture();
}
HI::HITexture HI::createTexture(int sizeX, int sizeY) {
	sf::Texture* texture = new sf::Texture;
	texture->create(sizeX, sizeY);
	return (HI::HITexture)texture;
}

void HI::endFrame() {
}

void HI::swapBuffers() {
	window->display();
}

std::string HI::getDataPath() {
	return "data/";
}

std::string HI::getSavesPath() {
	return "saves/";
}

bool HardwareInterface::createDir(std::string path) { //WIP
	return true;
}

bool HardwareInterface::fsExists(std::string path) {  //WIP
	return true;
}
bool HI::fsIsDirectory(const std::string path) {   //WIP
	return true;
}

bool HardwareInterface::copyFile(std::string origin, std::string destination) {
	std::ifstream  iFile(origin, std::ios::binary);
	std::ofstream  oFile(destination, std::ios::binary);
	oFile << iFile.rdbuf();
	oFile.close();
	iFile.close();
	return true;
}

int HI::getScreenHeight() {
	return window->getSize().y;
}
int HI::getScreenWidth() {
	return window->getSize().x;
}

HardwareInterface::HI_PLATFORM HI::getPlatform() {
	return PLATFORM_PC;
}

void HI::createThread(void* entrypoint, std::reference_wrapper<void(void*)> entrypoint2, void* arg, size_t stack_size, int prio, int affinity, bool detached, size_t arg_size) {
	std::thread newThread(entrypoint2.get(), arg);
	newThread.detach();
}

void HI::updateTouch(point2D &touch) {
	touch.x = sf::Mouse::getPosition(*window).x;
	touch.y = sf::Mouse::getPosition(*window).y;
}
void HI::updateHID() {
}

int HI::getKeysUp() {
	HI::HI_KEYS keys = HI::HI_KEY_B;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) keys = (HI_KEYS)((int)keys | HI::HI_KEY_DOWN);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) keys = (HI_KEYS)((int)keys | HI::HI_KEY_A);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))keys = (HI_KEYS)((int)keys | HI::HI_KEY_LEFT);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))keys = (HI_KEYS)((int)keys | HI::HI_KEY_RIGHT);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))keys = (HI_KEYS)((int)keys | HI::HI_KEY_UP);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))keys = (HI_KEYS)((int)keys | HI::HI_KEY_START);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))keys = (HI_KEYS)((int)keys | HI::HI_KEY_TOUCH);
	return keys;
}
int HI::getKeysHeld() {
	HI::HI_KEYS keys = HI::HI_KEY_B;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) keys = (HI_KEYS)((int)keys | HI::HI_KEY_DOWN);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) keys = (HI_KEYS)((int)keys | HI::HI_KEY_A);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))keys = (HI_KEYS)((int)keys | HI::HI_KEY_LEFT);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))keys = (HI_KEYS)((int)keys | HI::HI_KEY_RIGHT);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))keys = (HI_KEYS)((int)keys | HI::HI_KEY_UP);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))keys = (HI_KEYS)((int)keys | HI::HI_KEY_START);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))keys = (HI_KEYS)((int)keys | HI::HI_KEY_TOUCH);
	return keys;
}
int HI::getKeysDown() {
	HI::HI_KEYS keys = HI::HI_KEY_B;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) keys = (HI_KEYS)((int)keys | HI::HI_KEY_DOWN);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) keys = (HI_KEYS)((int)keys | HI::HI_KEY_A);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))keys = (HI_KEYS)((int)keys | HI::HI_KEY_LEFT);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))keys = (HI_KEYS)((int)keys | HI::HI_KEY_RIGHT);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))keys = (HI_KEYS)((int)keys | HI::HI_KEY_UP);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))keys = (HI_KEYS)((int)keys | HI::HI_KEY_START);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))keys = (HI_KEYS)((int)keys | HI::HI_KEY_TOUCH);
	return keys;
}

void HI::getCirclePadPos(point2D &circle, HI_CIRCLEPAD circlePadID) { //WIP
}

void HI::sleepThread(unsigned long ns) {	 //WIP
	std::this_thread::sleep_for(std::chrono::nanoseconds(ns));
}


void* HI::linearAllocator(size_t size) {   //WIP
	return ::malloc(size);
}

//Sound
void HI::dspSetOutputMode(dspOutputMode mode) {
}
void HI::dspChnSetInterp(int id, HI::dspInterpType type) {
}
void HI::dspChnSetFormat(int id, unsigned short format) {
}
void HI::dspChnSetRate(int id, float rate) {
}
void HI::dspChnSetMix(int id, float mix[12]) {
}
void HI::dspChnWaveBufAdd(int id, HI::dspWaveBuf* buf) {
}

//??
void HI::DSP_FlushDataCache(const void* address, unsigned int size) {
}

void HardwareInterface::debugPrint(string s) {
	//HardwareInterface::debugPrint(s, 1);
}

void HardwareInterface::debugPrint(string s, int priority) {
	//if (priority >= DEBUG_PRIORITY) cout << s;
}

void HI::gspWaitForEvent(HardwareInterface::GSPGPU_Event id, bool nextEvent) {
}

void HI::waitForVBlank() {
	std::this_thread::sleep_for(16666ns);
}

bool HI::aptMainLoop() {
	if (window->isOpen()) {
		while (window->pollEvent(*event)) {
			// "close requested" event: we close the window
			if (event->type == sf::Event::Closed) {
				window->close();
				return false;
			}
			if (event->type == sf::Event::Resized)
				window->setView(sf::View(sf::FloatRect(0.f, 0.f,
					static_cast<float>(window->getSize().x),
					static_cast<float>(window->getSize().y))));
		}
		return true;
	}
	else return false;
}
#endif