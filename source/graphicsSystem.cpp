#include "../include/graphicsSystem.h"
#include "../include/HardwareInterface.h"

using namespace std; 

graphicsSystem::~graphicsSystem() {
	freeAllTextures();
}

bool graphicsSystem::isTextureLoaded(string textureFile) const { // tells if a texture with said name is present on texTable
	return texAtlas.find(textureFile) != texAtlas.end();
}

HI::HITexture graphicsSystem::loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
	if (texAtlas.find(fileName) == texAtlas.end()) {
		texAtlas.insert(make_pair(fileName, HI::loadPngFile(HI::getDataPath() + "sprites/" + fileName.c_str())));
	}
	return texAtlas.find(fileName)->second;
}

void graphicsSystem::freeTexture(string fileName) { //frees a texture from texTable[]
	auto it = texAtlas.find(fileName);
	if (it != texAtlas.end()) {
		HI::freeTexture(it->second);
		texAtlas.erase(it);
	}
}

HI::HITexture graphicsSystem::getTexture(string fileName)
{
	auto it = texAtlas.find(fileName);
	if(it==texAtlas.end())
	{
		cout << "AAAAAAAAAAAAARG"<<endl;
	}
	else return it->second;
	
}

void graphicsSystem::freeAllTextures() {	 //frees all textures
	for (auto it : texAtlas) {
		HI::freeTexture(it.second);
		texAtlas.erase(it.first);
	}
}
