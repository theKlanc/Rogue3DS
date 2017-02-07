TITLE_ID = ROGUEVITA
TARGET   = rogueVita
SOURCES = $(wildcard source/*.cpp) $(wildcard source/*.c) $(wildcard source/entityx/*.cpp) $(wildcard source/entityx/*.c) $(wildcard source/entityx/help/*.cpp) $(wildcard source/entityx/help/*.c) $(wildcard source/states/*.cpp)
OBJECTS = $(SOURCES:*.cpp=*.o) 
OBJECTS += $(SOURCES:*.c=*.o)


LIBS = -lpthread -lvita2d -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub \
	-lSceCommonDialog_stub -lfreetype -lpng -ljpeg -lz -lm -lc \
	-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \
	-lSceNetCtl_stub -lScePower_stub -lSceAppMgr_stub -lSceTouch_stub

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-g++
CFLAGS  = -Wl,-q -std=c++14 -D_VITA
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/pic0.png=sce_sys/pic0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/logo0.png=sce_sys/livearea/contents/logo0.png \
		--add sce_sys/livearea/contents/logo1.png=sce_sys/livearea/contents/logo1.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
		$@

eboot.bin: $(TARGET).velf
	vita-make-fself $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^

%*.o: %*.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
	
%*.o: %*.c
	$(CC) $(CFLAGS) -c -o $@ $<

	
clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf \
		eboot.bin param.sfo