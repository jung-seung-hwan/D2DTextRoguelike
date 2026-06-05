#include "pch.h"
#include "CombatManager.h"
#include "Player.h"
#include "Monster.h"

void CombatManager::StartBattle(Player* player, Monster* monster)
{
	m_player = player;
	m_monster = monster;

	m_state = BattleState::PLAYERTURN;
	m_turnCount = 1;

}

void CombatManager::PlayerAction(PLAYERACTION action)
{
	if (m_state != BattleState::PLAYERTURN)
		return;

	switch (action)
	{
	case PLAYERACTION::ATTACK:
		m_player->Attack(*m_monster);
		break;

	case PLAYERACTION::DEFEND:
		m_player->Defend();
		break;
	}

	m_player->Attack(*m_monster);

	if (m_monster->isDead)
	{
		m_state = BATTLESTATE::VICTORY;
		return;
	}

	m_state = BATTLESTATE::ENEMYTURN;
	EnemyTurn();
}

