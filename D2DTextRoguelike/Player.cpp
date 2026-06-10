#include "pch.h"
#include "Player.h"

void Player::Heal(int amount)
{
	hp += amount;

	if (hp > maxHp)
		hp = maxHp;
}

void Player::Defend()
{
	shield += 5;
}