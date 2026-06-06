#include "GameObject.h"
#include <algorithm>

int GameObject::TakeDamage(int damage)
{
	int finalDamage = damage - defense;

	if (finalDamage < 1)
		finalDamage = 1;

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

//void GameObject::Defend() 
//{
//
//}

bool GameObject::IsDead() const
{
	return isDead;
}