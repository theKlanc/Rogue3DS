#pragma once
#include <string>
#include "graphics.h"

class UI
{
public:
	UI(graphics* g);
	~UI();
	void drawText(string text, int posX, int posY, int size, HI::HIColor color);
	void drawPanel(int posX, int posY, int sizeX, int sizeY, std::string style = "default");
private:
	graphics* graphicsObj;
	HI::HIFont font;
};

