APPNAME := Rogue3DS
BINDIR	:= bin
PLATFORM := linux
SOURCES := source source/entityx/help  
INCLUDE := include source include/entityx

BUILDDIR := build
SOURCES := $(SOURCES) deps/reactPhysics3D/src/collision/broadphase deps/reactPhysics3D/src/collision/narrowphase
SOURCES := $(SOURCES) deps/reactPhysics3D/src/collision/shapes

FLAGS    := -D__LINUX__ -Werror=return-type 
CCFLAGS  :=
CXXFLAGS := -std=c++20

LIBS    :=  -lpthread -lstdc++fs -lsfml-graphics -lsfml-window -lsfml-system -lfreetype

#YOU SHOULDN'T NEED TO MODIFY ANYTHING PAST THIS POINT
ifeq ($(PRECISE_DEBUG), 1)
FLAGS := $(FLAGS) -D_DEBUG -DDEBUG -D_GLIBCXX_DEBUG -ggdb3 -O0 -fstack-protector-all
BUILDTYPE := Debug
else ifeq ($(DEBUG), 1)
FLAGS := $(FLAGS) -D_DEBUG -DDEBUG -ggdb3 -Og -fstack-protector-all
BUILDTYPE := FastDebug
else
FLAGS := $(FLAGS) -flto -Ofast
BUILDTYPE := Release
endif

$(info $$BUILDTYPE is [${BUILDTYPE}])

CCFLAGS := $(FLAGS) $(CCFLAGS)
CXXFLAGS := $(FLAGS) $(CXXFLAGS)

TOPDIR ?= $(CURDIR)

CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/**/*.c)) $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/**/*.cpp)) $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
MODULEFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/**/*.cc)) $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cc))


$(info $(CFILES))
$(info $(CPPFILES))


CFILES	    := $(CFILES:$(SOURCES)/%=%)
CPPFILES	:= $(CPPFILES:$(SOURCES)/%=%)

OFILES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
OFILES := $(addprefix $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/,$(OFILES))

$(info $(OFILES))

CC   := /usr/lib/ccache/bin/gcc
CXX  := /usr/lib/ccache/bin/g++


INCLUDE := $(addprefix -I,$(INCLUDE))

#depend: .depend
#
#.depend: $(CFILES) $(CPPFILES)
#	rm -f ./.depend
#	$(CXX) $(CFLAGS) -MM $^ > ./.depend;
#
include $(foreach dir,$(INCLUDE),$(wildcard $(dir)/**/*.h)) $(foreach dir,$(INCLUDE),$(wildcard $(dir)/*.h)) $(foreach dir,$(INCLUDE),$(wildcard $(dir)/**/*.hpp)) $(foreach dir,$(INCLUDE),$(wildcard $(dir)/*.hpp))

DEPENDS := $(patsubst %.cpp,%.d,$(CPPFILES))

.PHONY: pre all clean

all: pre $(OFILES)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OFILES) $(LIBS) -o $(BINDIR)/$(PLATFORM)/$(BUILDTYPE)/$(APPNAME)

pre:
	mkdir -p $(BINDIR)/$(PLATFORM)/$(BUILDTYPE)/
	mkdir -p $(addprefix $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/,$(dir $(CFILES))) $(addprefix $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/,$(dir $(CPPFILES)))
#	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $(MODULEFILES)

-include $(DEPENDS)

$(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/%.o: %.c linux.mk
	$(CC) $(CCFLAGS) $(INCLUDE) $(LIBS) -c $< -o $@

$(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/%.o: %.cpp linux.mk
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LIBS) -MMD -MP -c $< -o $@


clean:
	rm -rf $(BUILDDIR)/$(PLATFORM)/$(BUILDTYPE)/
	rm -f $(APPNAME)
#	rm -rf gcm.cache
