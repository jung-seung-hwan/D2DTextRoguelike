#include "pch.h"
#include "GameObject.h"

int GameObject::TakeDamage(int damage)
{
    int roll = rand() % 100;

    // 회피 성공 시 식별자 -1 반환
    if (roll < evasion)
    {
        return -1;
    }

    float reductRate = defense / (defense + 100.0f);
    int finalDamage = static_cast<int>(damage * (1.0f - reductRate));

    if (finalDamage < 1) finalDamage = 1;

    if (shield > 0)
    {
        finalDamage -= shield;
        if (finalDamage < 0) finalDamage = 0; // 쉴드로 방어한 경우는 0 데미지
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

int GameObject::Attack(GameObject& target, float multiplier)
{
	int finalDamage = static_cast<int>(attack * multiplier);

	return target.TakeDamage(finalDamage);
}

void GameObject::Defend()
{
	// 최대 체력의 20% 방어막 생성 (1회용)
	shield = maxHp / 5;
}

void GameObject::Defend(int shieldAmount)
{
    if (shieldAmount < 0)
        shieldAmount = 0;

    shield = shieldAmount;
}

bool GameObject::IsDead() const
{
	return isDead;
}