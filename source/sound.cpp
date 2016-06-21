#include <string.h>
#include <3ds.h>
#include <iostream>
#include <stdio.h>
#include "sound.h"
#include "stb_vorbis.h"


using namespace std;



sound::sound()
{
	waveBuf = new ndspWaveBuf[2];
	ndspInit();

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);

	ndspChnSetInterp(0, NDSP_INTERP_LINEAR);

	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
	temp = false;

}

void sound::playFromFile(string file)
{

	stb_vorbis_info info;
	int error;


	vorbisFile = stb_vorbis_open_filename("/mau5.ogg", &error, NULL);
	info = stb_vorbis_get_info(vorbisFile);
	Samples = info.sample_rate;



	audioBuffer = (u32*)linearAlloc(Samples * sizeof(s16) * 2);

	fillBlock = false;



	ndspChnSetRate(0, Samples);
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(0, mix);

	memset(waveBuf, 0, sizeof(ndspWaveBuf)*2);
	waveBuf[0].data_vaddr = &audioBuffer[0];
	waveBuf[0].nsamples = Samples;
	waveBuf[1].data_vaddr = &audioBuffer[Samples];
	waveBuf[1].nsamples = Samples;

	ndspChnWaveBufAdd(0, &waveBuf[0]);
	ndspChnWaveBufAdd(0, &waveBuf[1]);

	temp = true;
	//threadCreate((ThreadFunc)audioMainThread, (void*)this, 100, 0x2F, -2, false);


}

void sound::update()
{
	if (waveBuf[fillBlock].status == NDSP_WBUF_DONE) {

		stb_vorbis_get_samples_short_interleaved(vorbisFile, 2, (short*)waveBuf[fillBlock].data_pcm16, Samples * 2);

		DSP_FlushDataCache(&waveBuf[fillBlock].data_pcm16, waveBuf[fillBlock].nsamples);


		ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);

		fillBlock = !fillBlock;
	}

}

//void sound::audioMainThread(u32 arg)
//{
//	sound *mainObj = (sound*)arg;
//	while (1) {
//		if (waveBuf[fillBlock].status == NDSP_WBUF_DONE) {
//
//			u32 *dest = (u32*)&waveBuf[fillBlock].data_pcm16;
//
//			stb_vorbis_get_samples_short_interleaved(vorbisFile, 2, (short*)dest, Samples * 2);
//
//			DSP_FlushDataCache(&waveBuf[fillBlock].data_pcm16, waveBuf[fillBlock].nsamples);
//
//
//			ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);
//
//			fillBlock = !fillBlock;
//		}
//		else  svcSleepThread(50000000);
//	}
//}

