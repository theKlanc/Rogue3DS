/*
	TO-DO
	Afegir SFX
	Afegir més canals
	evitar crear dos threads al mateix canal

*/

#pragma once
#include <string>
#include "stb_vorbis.h"
#include "HardwareInterface.h"

using namespace std;

class sound
{
public:
	sound();
	~sound();
	void playFromFile(string file);
	void exit();
private:
	short assignChannel();
	bool channelStatus[8];
	bool exitRequest;
	bool threadStatus;
	static void audioMainThread(unsigned int arg);
	int *audioBuffer;
	int fillBlock;
	HI::dspWaveBuf *waveBuf;
	stb_vorbis* vorbisFile;
	int Samples;
};