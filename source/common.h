#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <sstream>
#ifdef _WIN32
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#else
#include "sf2d.h"
#endif

using namespace std;

#define CHUNK_SIZE 100
#define CHUNK_NUM  19 //( 19 = rubik's sense corners (n=2) - corners, 11 = baldufa(n = 2, però z pondera més) , 7 = rubik's core (n = 1))  la n3ds aguanta almenys 80, la old 26
#define ENTITY_LIST_SIZE  100
#define TERRAIN_LIST_MAX_SIZE  100
#define TEX_TABLE_SIZE  30
#define RENDER_HEIGHT 1
#define FLOOR_HEIGHT 550



enum mode {
	PRRT,
	TRRN,
	NTT,
	RAY,
	BLCK,
};

enum nttype {
	NPC,
	PLR,
};

enum direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FRONT,
	BACK,
};
struct point3D {
	int x = -1;
	int y = -1;
	int z = -1;
};
struct entity {
	string spriteName = "ENULL";
	bool visible = 1;
	bool solid = 1;
	int posX = -1;
	int posY = -1;
	int posZ = -1;
	nttype type = NPC;
};

string get_string(int number);

struct textureName {
#ifdef _WIN32
	sf::Texture texture;
#else
	sf2d_texture* texture;
#endif
	string name = "free";
};

struct terrain {
	string textureFile = "TNULL";
	bool visible = true;
	bool solid = true;
};
#endif