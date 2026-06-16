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

void CombatManager::SetAction(PLAYERACTION action)
{
	if (m_state != BATTLESTATE::PLAYERTURN)
		return;

	m_pendingAction = action;
}

void CombatManager::ApplyDiceResult(const DiceSystem::Result& result)
{
	if (m_state != BATTLESTATE::PLAYERTURN)
		return;

	m_playerDice = result;
	PlayerAction(m_pendingAction);
}


void CombatManager::PlayerAction(PLAYERACTION action)
{
	if (m_state != BATTLESTATE::PLAYERTURN)
		return;

	switch (action)
	{
	case PLAYERACTION::ATTACK:
		PlayerAttack();
		break;

	case PLAYERACTION::DEFEND:
		PlayerDefend();
		break;

	case PLAYERACTION::SKILL:
		PlayerSkill();
		break;

	case PLAYERACTION::ITEM:
		PlayerItem();
		break;
	}

	if (m_monster->isDead)
	{
		m_state = BATTLESTATE::VICTORY;
		return;
	}

	m_state = BATTLESTATE::ENEMYTURN;
}




void CombatManager::PlayerAttack()
{

	if (m_playerDice.IsFumble())
	{
		int recoilDamage = m_player->attack / 2;

		m_damageToMonster = 0;
		m_damageToPlayer = m_player->TakeDamage(recoilDamage);

		return;
	}

	m_damageToMonster = m_player->Attack(
		*m_monster,
		m_playerDice.totalMultiplier
	);
}


void CombatManager::PlayerDefend()
{
	m_player->Defend();
}



void CombatManager::PlayerSkill()
{
	int skillDamage = m_player->attack * 2;

	m_damageToMonster = m_monster->TakeDamage(skillDamage);
}



void CombatManager::PlayerItem()
{

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
