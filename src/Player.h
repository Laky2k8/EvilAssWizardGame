#pragma once

#include "raylib.h"

class Player
{
	
public:

	Vector2 position;
	float speed;

	float angle;
	Vector2 deltaPos;

	float fov;

	Player(Vector2 position, float speed, float fov) : position(position), speed(speed), fov(fov) {}

};