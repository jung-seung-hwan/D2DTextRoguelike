#include "GameObject.h"

int GameObject::TakeDamage(int damage)
{
	int roll = rand() % 100;

	if (roll < evasion)
	{
		return 0;
	}

	float reductRate = defense / (defense + 100.0f);
	int finalDamage = static_cast<int>(damage * (1.0f - reductRate));

	if (finalDamage < 1)
		finalDamage = 1;

	if (shield > 0)
	{
		finalDamage -= shield;

		if (finalDamage < 0)
			finalDamage = 0;

		shield = 0;
	}

	hp -= finalDamage;

	if (hp <= 0)
	{
		hp = 0;
		isDead = true;
	}

	return finalDamage;
}

int GameObject::Attack(GameObject& target)
{
	return target.TakeDamage(attack);
}

void GameObject::Defend()
{
	// 최대 체력의 20% 방어막 생성 (1회용)
	shield = maxHp / 5;
}

bool GameObject::IsDead() const
{
	return isDead;
}