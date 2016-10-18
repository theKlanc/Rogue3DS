#pragma once
#include "string.h"
#include "core.h"

using namespace std;

struct  Animation{
	explicit Animation(int frames, string filename) : frames(frames), filename(filename) {}

	int frames;
	string filename;
};

struct  FixedSprite {
	explicit FixedSprite(string filename) : filename(filename) {}
	bool isLoaded= false;
	int texPos;
	string filename;
};

struct  AIFollower {
	explicit AIFollower(point3D* target, int dist) : dist(dist), target(target){}

	int dist;
	point3D* target;
};

struct Health
{
	explicit Health(int maxHealth, int currentHealth) : currentHealth(currentHealth), maxHealth(maxHealth){}

	int currentHealth;
	int maxHealth;
};

struct Stamina
{
	explicit Stamina(int maxStamina, int currentStamina) : currentStamina(currentStamina), maxStamina(maxStamina) {}

	int currentStamina;
	int maxStamina;
};

struct Velocity
{
	explicit Velocity(point3D currentVelocity) : currentVelocity(currentVelocity){}

		point3D currentVelocity;
};
struct Position
{
	explicit Position(point3D currentPosition) : currentPosition(currentPosition) {}

	point3D currentPosition;
};

struct Collision{};
struct CanSwim{};
struct CanFly{};
struct Player
{
	explicit Player(point3D* pos) : pos(pos){}
	point3D* pos;
};

struct Inventory{};