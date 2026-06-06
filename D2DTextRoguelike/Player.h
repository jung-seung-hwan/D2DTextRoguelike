#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
	int gold = 0;
	int shield = 0;

public:
	void Heal(int amount);
	void Defend();
};

