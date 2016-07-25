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
}

void sound::playFromFile(string file)
{

	stb_vorbis_info info;
	int error;


	vorbisFile = stb_vorbis_open_filename(file.c_str(), &error, NULL);
	info = stb_vorbis_get_info(vorbisFile);
	Samples = info.sample_rate;



	audioBuffer = (u32*)linearAlloc(Samples * sizeof(s16) * 2);

	fillBlock = 0;



	ndspChnSetRate(0, Samples);
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(0, mix);

	memset(waveBuf, 0, sizeof(ndspWaveBuf) * 2);
	waveBuf[0].data_vaddr = &audioBuffer[0];
	waveBuf[0].nsamples = Samples;
	waveBuf[1].data_vaddr = &audioBuffer[Samples];
	waveBuf[1].nsamples = Samples;

	ndspChnWaveBufAdd(0, &waveBuf[0]);
	ndspChnWaveBufAdd(0, &waveBuf[1]);
	threadCreate((ThreadFunc)audioMainThread,this, 5900, 0x30, 0, false);
}

//void sound::update()
//{
//	if (waveBuf[fillBlock].status == NDSP_WBUF_DONE) {
//
//		stb_vorbis_get_samples_short_interleaved(vorbisFile, 2, (short*)waveBuf[fillBlock].data_pcm16, Samples * 2);
//
//		DSP_FlushDataCache(&waveBuf[fillBlock].data_pcm16, waveBuf[fillBlock].nsamples);
//
//
//		ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);
//
//		fillBlock++;
//		if (fillBlock > 1)fillBlock = 0;
//	} 
//}

void sound::exit()
{
	delete[] waveBuf;
}

//void sound::audioMainThread(u32 arg)
//{
//	threadArg2 *mainObj = (threadArg2*)arg;
//	while (1) {
//		if (mainObj->waveBuf[*mainObj->fillBlock]->status == NDSP_WBUF_DONE) {
//
//			u32 *dest = (u32*)mainObj->waveBuf[*mainObj->fillBlock]->data_pcm16;
//
//			stb_vorbis_get_samples_short_interleaved(mainObj->vorbisFile, 2, (short*)dest, *mainObj->Samples * 2);
//
//			DSP_FlushDataCache(mainObj->waveBuf[*mainObj->fillBlock]->data_pcm16, mainObj->waveBuf[*mainObj->fillBlock]->nsamples);
//
//
//			ndspChnWaveBufAdd(0, mainObj->waveBuf[*mainObj->fillBlock]);
//
//			*mainObj->fillBlock = !*mainObj->fillBlock;
//		}
//		else  svcSleepThread(50000000);
//	}
//}
void sound::audioMainThread(u32 arg)
{
	sound* soundObj = (sound*)arg;
	while (1) {
		if (soundObj->waveBuf[soundObj->fillBlock].status == NDSP_WBUF_DONE) {

			stb_vorbis_get_samples_short_interleaved(soundObj->vorbisFile, 2, (short*)soundObj->waveBuf[soundObj->fillBlock].data_pcm16, soundObj->Samples * 2);

			DSP_FlushDataCache(&soundObj->waveBuf[soundObj->fillBlock].data_pcm16, soundObj->waveBuf[soundObj->fillBlock].nsamples);


			ndspChnWaveBufAdd(0, &soundObj->waveBuf[soundObj->fillBlock]);

			soundObj->fillBlock++;
			if (soundObj->fillBlock > 1)soundObj->fillBlock = 0;
		}
		svcSleepThread(50000000);
	}
}