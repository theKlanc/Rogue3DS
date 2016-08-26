/*
	TO-DO
	Afegir SFX
	Afegir més canals
	evitar crear dos threads al mateix canal
	
*/

#pragma once
#include <3ds.h>
#include <string>
#include "stb_vorbis.h"

using namespace std;

class sound
{
public:
	sound();
	~sound();
	void playFromFile(string file);
	void exit();
private:
	static void audioMainThread(u32 arg);
	Thread audioThread;
	u32 *audioBuffer;
	int fillBlock;
	ndspWaveBuf *waveBuf;
	stb_vorbis* vorbisFile;
	u32 Samples;
};