#include "pch.h"
#include "Player.h"

Player::Player()
{
    Reset(); // 객체 생성 시 기본 스탯 적용
}

// 기본 스텟 설정
void Player::Reset()
{
    m_inventory.Clear();
    name = L"플레이어";
    maxHp = 100;
    hp = maxHp;
    attack = 5000;
    defense = 10;
    evasion = 10;
    isDead = false;
    gold = 0;
    shield = 0;
}

void Player::Heal(int amount)
{
	hp += amount;

	if (hp > maxHp)
		hp = maxHp;
}