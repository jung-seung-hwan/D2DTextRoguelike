#include "Monster.h"

void Monster::InitFromData(const MonsterData& data)
{
	monsterId = data.id;

	name = data.name;

	maxHp = data.maxHp;
	hp = maxHp;

	attack = data.attack;

	isDead = false;
}