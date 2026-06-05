#pragma once

#include "GameObject.h"

class Player : public GameObject
{
public:
	int gold = 0;

public:
	void Heal(int amount);
};

