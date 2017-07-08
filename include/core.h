#pragma once

#include <string.h>
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

#define CHUNK_SIZE 64
#define CHUNK_NUM  27 //( 19 = rubik's sense corners (n=2) - corners, 11 = baldufa(n = 2, però z pondera més) , 7 = rubik's core (n = 1))  la n3ds aguanta almenys 80, la old 26
#define ENTITY_LIST_SIZE  100
#define TERRAIN_LIST_MAX_SIZE  100
#define TEX_TABLE_SIZE  30
#define RENDER_HEIGHT 3
#define FLOOR_HEIGHT 100
#define SEA_LEVEL 150

#define PI		3.14159265358979323846

#define TICKS_PER_SEC (268123480)
#define TICKS_PER_FRAME (4468724)

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
	point3D(int x, int y, int z) : x(x),y(y),z(z){}
	point3D(){}
	int x = -1;
	int y = -1;
	int z = -1;
	bool inRange(point3D p, int dist)
	{
		return (abs(p.x - x) < dist && abs(p.y - y) < dist && abs(p.y - y) < dist);
	}
};
struct point2D {
	int x = -1;
	int y = -1;
};

struct entity {
	string spriteName = "ENULL";
	bool visible = true;
	bool solid = true;
	point3D pos;
	nttype type = NPC;
	bool fly = false;
};

string get_string(int number);

struct terrain {
	string textureFile = "TNULL";
	bool visible = false;
	bool solid = false;
	bool opaque = false;
	bool canFloatInIt = false;
};

bool fsIsDirectory(const std::string path);
bool fsExists(const std::string path);
bool fsCreateDir(const std::string path);
bool testing();
