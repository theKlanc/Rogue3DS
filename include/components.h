#pragma once
#include "string.h"
#include "core.h"

using namespace std;

struct  Animation {
	explicit Animation(int frames, string filename) : frames(frames), filename(filename) {}

	int frames;
	string filename;
};

struct  FixedSprite {
	explicit FixedSprite(string filename) : filename(filename) {}
	bool isLoaded = false;
	int texPos;
	string filename;
};

struct Health {
	explicit Health(int maxHealth, int currentHealth) : currentHealth(currentHealth), maxHealth(maxHealth) {}

	int currentHealth;
	int maxHealth;
};

struct Stamina {
	explicit Stamina(int maxStamina, int currentStamina) : currentStamina(currentStamina), maxStamina(maxStamina) {}

	int currentStamina;
	int maxStamina;
};

struct Velocity {
	explicit Velocity(point3D currentVelocity) : currentVelocity(currentVelocity) {}

	point3D currentVelocity;
};
struct Position {
	explicit Position(point3D currentPosition) : currentPosition(currentPosition) {}

	point3D currentPosition;
};

struct  AIFollower {
	explicit AIFollower(Position* target, int dist) : dist(dist), target(target) {}

	int dist;
	Position* target;
};

struct Jump {
	explicit Jump() : jumping(false) {}
	bool jumping;
};

struct canFloat {
	explicit canFloat() : floating(false) {}
	bool floating;
};
struct Name {
	explicit Name(string name) : name(name) {}
	string name;
};


struct autoFloat {};
struct AIDrunk {};
struct Collision {};
struct CanSwim {};
struct CanFly {};
struct Player {};
struct Inventory {};