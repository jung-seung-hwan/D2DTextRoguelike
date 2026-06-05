#include "GameObject.h"
#include <algorithm>

void GameObject::TakeDamage(int damage)
{
	int finalDamage = damage - defense;

	if (finalDamage < 1)
		finalDamage = 0;

	hp -= finalDamage;

	if (hp <= 0)
	{
		hp = 0;
		isDead = true;
	}
}

void GameObject::Attack(GameObject& target)
{
	target.TakeDamage(attack);
}

void GameObject::Defend() 
{

}

bool GameObject::IsDead() const
{
	return isDead;
}