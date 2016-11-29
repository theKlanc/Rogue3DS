#pragma once
#include <string>
#include "core.h"

#define RGBA8(r, g, b, a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))
#define BIT(n) (1U<<(n))

namespace HardwareInterface
{
	typedef void* HITexture;
	typedef int HIColor;
	enum HI_PLATFORM {
		PLATFORM_PSVITA,
		PLATFORM_NINTENDO3DS,
		PLATFORM_PC,
	};
	enum HI_SCREEN
	{
		SCREEN_TOP,
		SCREEN_BOT
	};
	enum HI_CIRCLEPAD
	{
		CIRCLEPAD_LEFT,
		CIRCLEPAD_RIGHT
	};
	enum HI_KEYS
	{
		HI_KEY_A = BIT(0),       ///< A
		HI_KEY_B = BIT(1),       ///< B
		HI_KEY_SELECT = BIT(2),       ///< Select
		HI_KEY_START = BIT(3),       ///< Start
		HI_KEY_DRIGHT = BIT(4),       ///< D-Pad Right
		HI_KEY_DLEFT = BIT(5),       ///< D-Pad Left
		HI_KEY_DUP = BIT(6),       ///< D-Pad Up
		HI_KEY_DDOWN = BIT(7),       ///< D-Pad Down
		HI_KEY_R = BIT(8),       ///< R
		HI_KEY_L = BIT(9),       ///< L
		HI_KEY_X = BIT(10),      ///< X
		HI_KEY_Y = BIT(11),      ///< Y
		HI_KEY_ZL = BIT(14),      ///< ZL (New 3DS only)
		HI_KEY_ZR = BIT(15),      ///< ZR (New 3DS only)
		HI_KEY_TOUCH = BIT(20),      ///< Touch (Not actually provided by HID)
		HI_KEY_CSTICK_RIGHT = BIT(24), ///< C-Stick Right (New 3DS only)
		HI_KEY_CSTICK_LEFT = BIT(25), ///< C-Stick Left (New 3DS only)
		HI_KEY_CSTICK_UP = BIT(26), ///< C-Stick Up (New 3DS only)
		HI_KEY_CSTICK_DOWN = BIT(27), ///< C-Stick Down (New 3DS only)
		HI_KEY_CPAD_RIGHT = BIT(28),   ///< Circle Pad Right
		HI_KEY_CPAD_LEFT = BIT(29),   ///< Circle Pad Left
		HI_KEY_CPAD_UP = BIT(30),   ///< Circle Pad Up
		HI_KEY_CPAD_DOWN = BIT(31),   ///< Circle Pad Down

								   // Generic catch-all directions
								   HI_KEY_UP = HI_KEY_DUP | HI_KEY_CPAD_UP,    ///< D-Pad Up or Circle Pad Up
								   HI_KEY_DOWN = HI_KEY_DDOWN | HI_KEY_CPAD_DOWN,  ///< D-Pad Down or Circle Pad Down
								   HI_KEY_LEFT = HI_KEY_DLEFT | HI_KEY_CPAD_LEFT,  ///< D-Pad Left or Circle Pad Left
								   HI_KEY_RIGHT = HI_KEY_DRIGHT | HI_KEY_CPAD_RIGHT ///< D-Pad Right or Circle Pad Right
	};
	struct  dspAdpcmData
	{
		unsigned short index;    ///< Current predictor index
		short history0; ///< Last outputted PCM16 sample.
		short history1; ///< Second to last outputted PCM16 sample.
	};
	struct dspWaveBuf
	{
		union
		{
			char*         data_pcm8;  ///< Pointer to PCM8 sample data.
			short*        data_pcm16; ///< Pointer to PCM16 sample data.
			unsigned char*         data_adpcm; ///< Pointer to DSPADPCM sample data.
			const void* data_vaddr; ///< Data virtual address.
		};
		unsigned int nsamples;              ///< Total number of samples (PCM8=bytes, PCM16=halfwords, DSPADPCM=nibbles without frame headers)
		dspAdpcmData* adpcm_data; ///< ADPCM data.

		unsigned int  offset;  ///< Buffer offset. Only used for capture.
		bool looping; ///< Whether to loop the buffer.
		unsigned char   status;  ///< Queuing/playback status.

		unsigned short sequence_id;   ///< Sequence ID. Assigned automatically by ndspChnWaveBufAdd.
		dspWaveBuf* next; ///< Next buffer to play. Used internally, do not modify.
	};
	enum dspClippingMode
	{
		DSP_CLIP_NORMAL = 0, ///< "Normal" clipping mode (?)
		DSP_CLIP_SOFT = 1, ///< "Soft" clipping mode (?)
	};
	enum dspOutputMode
	{
		DSP_OUTPUT_MONO = 0, ///< Mono sound
		DSP_OUTPUT_STEREO = 1, ///< Stereo sound
		DSP_OUTPUT_SURROUND = 2, ///< 3D Surround sound
	};
	enum dspInterpType {
		DSP_INTERP_POLYPHASE = 0, ///< Polyphase interpolation
		DSP_INTERP_LINEAR = 1, ///< Linear interpolation
		DSP_INTERP_NONE = 2, ///< No interpolation
	};
	enum
	{
		DSP_WBUF_FREE = 0, ///< The wave buffer is not queued.
		DSP_WBUF_QUEUED = 1, ///< The wave buffer is queued and has not been played yet.
		DSP_WBUF_PLAYING = 2, ///< The wave buffer is playing right now.
		DSP_WBUF_DONE = 3, ///< The wave buffer has finished being played.
	};
	enum
	{
		DSP_ENCODING_PCM8 = 0, ///< PCM8
		DSP_ENCODING_PCM16,    ///< PCM16
		DSP_ENCODING_ADPCM,    ///< DSPADPCM (GameCube format)
	};

	/// Specifies the number of channels used in a sample.
#define DSP_CHANNELS(n)  ((unsigned int)(n) & 3)
	/// Specifies the encoding used in a sample.
#define DSP_ENCODING(n) (((unsigned int)(n) & 3) << 2)

	/// Channel format flags for use with ndspChnSetFormat.
	enum
	{
		DSP_FORMAT_MONO_PCM8 = DSP_CHANNELS(1) | DSP_ENCODING(DSP_ENCODING_PCM8),  ///< Buffer contains Mono   PCM8.
		DSP_FORMAT_MONO_PCM16 = DSP_CHANNELS(1) | DSP_ENCODING(DSP_ENCODING_PCM16), ///< Buffer contains Mono   PCM16.
		DSP_FORMAT_MONO_ADPCM = DSP_CHANNELS(1) | DSP_ENCODING(DSP_ENCODING_ADPCM), ///< Buffer contains Mono   ADPCM.
		DSP_FORMAT_STEREO_PCM8 = DSP_CHANNELS(2) | DSP_ENCODING(DSP_ENCODING_PCM8),  ///< Buffer contains Stereo PCM8.
		DSP_FORMAT_STEREO_PCM16 = DSP_CHANNELS(2) | DSP_ENCODING(DSP_ENCODING_PCM16), ///< Buffer contains Stereo PCM16.

		DSP_FORMAT_PCM8 = DSP_FORMAT_MONO_PCM8,  ///< (Alias) Buffer contains Mono PCM8.
		DSP_FORMAT_PCM16 = DSP_FORMAT_MONO_PCM16, ///< (Alias) Buffer contains Mono PCM16.
		DSP_FORMAT_ADPCM = DSP_FORMAT_MONO_ADPCM, ///< (Alias) Buffer contains Mono ADPCM.

													// Flags
													DSP_FRONT_BYPASS = BIT(4), ///< Front bypass.
													DSP_3D_SURROUND_PREPROCESSED = BIT(6), ///< (?) Unknown, under research
	};
	typedef unsigned int HISize;
	//System
	void systemInit();
	void systemFini();
	void consoleInit();
	void consoleFini();
	void sleepThread(unsigned long ns);
	void* linearAllocator(size_t size);
	bool aptMainLoop();

	//input
	void updateHID();
	int getKeysDown();
	int getKeysUp();
	int getKeysHeld();
	void updateTouch(point2D &touch);
	void getCirclePadPos(point2D &circle, HI_CIRCLEPAD circlePadID);

	//graphics
	short getRComponent(HIColor color);
	short getGComponent(HIColor color);
	short getBComponent(HIColor color);
	short getAComponent(HIColor color);

	void startFrame(HI_SCREEN screen);
	void setBackgroundColor(HIColor color);
	HITexture loadPngFile(std::string path);
	HITexture loadBmpFile(std::string path);
	void drawTexture(HITexture texture, int posX, int posY);
	void drawTexturePart(HITexture texture, int startX, int startY, int posX, int posY, int sizeX, int sizeY);
	void drawTextureRotate(HITexture texture, int posX, int posY, float angle);
	void mergeTextures(HITexture originTexture, HITexture destinationTexture, short posX, short posY);
	void drawRectangle(int posX, int posY, int width, int height, HIColor color);
	HITexture createTexture(int sizeX, int sizeY);
	void freeTexture(HITexture texture);
	void endFrame();
	void swapBuffers();

	//filesystem
	std::string getDataPath();
	std::string getSavesPath();
	bool createDir(std::string path);
	bool fsExists(std::string path);
	bool fsIsDirectory(const std::string path);
	bool copyFile(std::string origin, std::string destination);

	//HardwareInfo
	int getScreenHeight();
	int getScreenWidth();
	HI_PLATFORM getPlatform();

	//Sound
	void dspSetOutputMode(dspOutputMode mode);
	void dspChnSetInterp(int id, dspInterpType type);
	void dspChnSetFormat(int id, unsigned short format);
	void dspChnSetRate(int id, float rate);
	void dspChnSetMix(int id, float mix[12]);
	void dspChnWaveBufAdd(int id, dspWaveBuf* buf);

	//??
	void DSP_FlushDataCache(const void* address, unsigned int size);

	//DEBUGGING
	void debugPrint(string s);
	void debugPrint(string s, int priority);

	//GSP
	enum  GSPGPU_Event
	{
		GSPGPU_EVENT_PSC0 = 0, ///< Memory fill completed.
		GSPGPU_EVENT_PSC1,     ///< TODO
		GSPGPU_EVENT_VBlank0,  ///< TODO
		GSPGPU_EVENT_VBlank1,  ///< TODO
		GSPGPU_EVENT_PPF,      ///< Display transfer finished.
		GSPGPU_EVENT_P3D,      ///< Command list processing finished.
		GSPGPU_EVENT_DMA,      ///< TODO

		GSPGPU_EVENT_MAX,      ///< Used to know how many events there are.
	};
	void gspWaitForEvent(HardwareInterface::GSPGPU_Event id, bool nextEvent);
	void waitForVBlank();
	//threads
	void createThread(void* entrypoint, void* arg, size_t stack_size, int prio, int affinity, bool detached, size_t arg_size);

	//static vars
}
namespace HI = HardwareInterface;

struct textureName {
	HI::HITexture texture  = NULL;
	string name = "";
};