#include "../include/UI.h"
#include "../include/graphics.h"
#include "../include/gameMap.h"

UI::UI(graphics* g): graphicsObj(g)
{
	font = HI::loadFont(HI::getDataPath() + "fonts/fredokaOne.ttf");
}

UI::~UI() {
	HI::freeFont(font);
}

void UI::drawText(string text, int posX, int posY, int size, HI::HIColor color)
{
	HI::drawText(font, text, posX, posY, size, color);
}

void UI::drawPanel(int posX, int posY, int sizeX, int sizeY, std::string style) {
	HI::HITexture TLcorner = graphicsObj->loadTexture(style + "_TLcorner.png");
	HI::HITexture BLcorner = graphicsObj->loadTexture(style + "_BLcorner.png");
	HI::HITexture TRcorner = graphicsObj->loadTexture(style + "_TRcorner.png");
	HI::HITexture BRcorner = graphicsObj->loadTexture(style + "_BRcorner.png");
	HI::HITexture TopBorder = graphicsObj->loadTexture(style + "_TopBorder.png");
	HI::HITexture BottomBorder = graphicsObj->loadTexture(style + "_BottomBorder.png");
	HI::HITexture LeftBorder = graphicsObj->loadTexture(style + "_LeftBorder.png");
	HI::HITexture RightBorder = graphicsObj->loadTexture(style + "_RightBorder.png");
	HI::HITexture background = graphicsObj->loadTexture(style + "_background.png");
	HI::drawTexture(TLcorner, posX, posY);
	HI::drawTexture(TRcorner, posX + sizeX - 16, posY);
	HI::drawTexture(BLcorner, posX, posY + sizeY - 16);
	HI::drawTexture(BRcorner, posX + sizeX - 16, posY + sizeY - 16);
	for (int i = 16; i < sizeX-16; i += 16) {
		HI::drawTexture(TopBorder, posX + i, posY);
		HI::drawTexture(BottomBorder, posX + i, posY + sizeY - 16);
	}
	for (int i = 16; i < sizeY-16; i += 16) {
		HI::drawTexture(LeftBorder, posX, posY+i);
		HI::drawTexture(RightBorder, posX+sizeX-16, posY+i);
	}
	for(int i = 16; i < sizeX - 16; i+=16)
	{
		for (int j = 16; j < sizeY - 16; j += 16) {
			HI::drawTexture(background, posX +i, posY + j);
		}
	}
}
