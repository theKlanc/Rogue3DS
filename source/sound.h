#pragma once
#include <3ds.h>
#include <string>
#include "stb_vorbis.h"

using namespace std;

class sound
{
public:
	sound();
	void playFromFile(string file);
	void update();
private:
	//static void audioMainThread(u32 arg);
	Thread audioThread;
	u32 *audioBuffer;
	bool fillBlock;
	ndspWaveBuf *waveBuf;
	stb_vorbis* vorbisFile;
	u32 Samples;
	bool temp;
};

