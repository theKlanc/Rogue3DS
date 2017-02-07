 #pragma once
#include "entityx/entityx.h"
#include "HardwareInterface.h"

namespace HI = HardwareInterface;

class graphics
{

public:
	graphics(){}
	~graphics();
	bool isTextureLoaded(string textureFile) const;
	HI::HITexture loadTexture(string fileName);
	void freeTexture(string fileName);
	HI::HITexture getTexture(string fileName);
private:
	
	void freeAllTextures();
	std::unordered_map<string, HI::HITexture> texAtlas;
};
