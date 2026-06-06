#include "pch.h"
#include "CombatManager.h"
#include "Player.h"
#include "Monster.h"

void CombatManager::StartBattle(Player* player, Monster* monster)
{
	m_player = player;
	m_monster = monster;

	m_state = BATTLESTATE::PLAYERTURN;
	m_turnCount = 1;

}

void CombatManager::PlayerAction(PLAYERACTION action)
{
	if (m_state != BATTLESTATE::PLAYERTURN)
		return;

	switch (action)
	{
	case PLAYERACTION::ATTACK:
		m_damageToMonster = m_player->Attack(*m_monster);
		break;

	case PLAYERACTION::DEFEND:
		//m_player->Defend();
		break;

	case PLAYERACTION::SKILL:
		// 나중에 스킬 구현 예정
		break;

	case PLAYERACTION::ITEM:
		// 나중에 아이템 구현 예정
		break;
	}

	if (m_monster->isDead)
	{
		m_state = BATTLESTATE::VICTORY;
		return;
	}

	m_state = BATTLESTATE::ENEMYTURN;
	EnemyTurn();
}

void CombatManager::EnemyTurn()
{
	if (m_state != BATTLESTATE::ENEMYTURN)
		return;

	m_damageToPlayer = m_monster->Attack(*m_player);

	if (m_player->isDead)
	{
		m_state = BATTLESTATE::DEFEAT;
		return;
	}

	m_turnCount++;
	m_state = BATTLESTATE::PLAYERTURN;
}

bool CombatManager::IsBattleEnd() const
{
	return m_state == BATTLESTATE::VICTORY ||
		m_state == BATTLESTATE::DEFEAT;
}

BATTLESTATE CombatManager::GetState() const
{
	return m_state;
}
