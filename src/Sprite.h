#pragma once

#include "raylib.h"

class Sprite
{
	
public:

	int type;
	int state;
	Texture2D texture;
	Vector2 position;

	Sprite(int type, Texture2D texture, Vector2 position) : type(type), texture(texture), position(position)
	{
		this->state = 0;
	}

};