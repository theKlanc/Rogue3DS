#include "../include/graphics.h"
#include "../include/HardwareInterface.h"

using namespace std; 

graphics::~graphics() {
	freeAllTextures();
}

bool graphics::isTextureLoaded(string textureFile) const { // tells if a texture with said name is present on texTable
	return texAtlas.find(textureFile) != texAtlas.end();
}

HI::HITexture graphics::loadTexture(string fileName) { //load a texture from a file into the first free space inside texTable[]
	if (texAtlas.find(fileName) == texAtlas.end()) {
		texAtlas.insert(make_pair(fileName, HI::loadPngFile(HI::getDataPath() + "sprites/" + fileName.c_str())));
	}
	return texAtlas.find(fileName)->second;
}

void graphics::freeTexture(string fileName) { //frees a texture from texTable[]
	auto it = texAtlas.find(fileName);
	if (it != texAtlas.end()) {
		HI::freeTexture(it->second);
		texAtlas.erase(it);
	}
}

HI::HITexture graphics::getTexture(string fileName)
{
	auto it = texAtlas.find(fileName);
	if(it==texAtlas.end())
	{
		cout << "AAAAAAAAAAAAARG"<<endl;
	}
	else return it->second;
	
}

void graphics::freeAllTextures() {	 //frees all textures
	for (auto it : texAtlas) {
		HI::freeTexture(it.second);
		texAtlas.erase(it.first);
	}
}
