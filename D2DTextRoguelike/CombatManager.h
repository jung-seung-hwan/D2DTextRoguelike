#pragma once
#include "DiceSystem.h"

class Player;
class Monster;

enum class BATTLESTATE
{
	NONE,
	PLAYERTURN,
	ENEMYTURN,
	VICTORY,
	DEFEAT
};

enum class PLAYERACTION
{
	ATTACK,
	DEFEND,
	SKILL,
	ITEM
};

class CombatManager
{
public:
	void StartBattle(Player* player, Monster* monster);

	void SetAction(PLAYERACTION action);
	void RollDice();
	void PlayerAction(PLAYERACTION action);

	void EnemyTurn();

	bool IsBattleEnd() const;
	BATTLESTATE GetState() const;

	int GetDamageToMonster() const
	{
		return m_damageToMonster;
	}

	int GetDamageToPlayer() const
	{
		return m_damageToPlayer;
	}

	const DiceSystem::Result& GetPlayerDice() const
	{
		return m_playerDice;
	}

	const DiceSystem::Result& GetEnemyDice() const
	{
		return m_enemyDice;
	}

private:
	void PlayerAttack();
	void PlayerDefend();
	void PlayerSkill();
	void PlayerItem();

private:
	Player* m_player = nullptr;
	Monster* m_monster = nullptr;

	PLAYERACTION m_pendingAction = PLAYERACTION::ATTACK;
	BATTLESTATE m_state = BATTLESTATE::NONE;
	int m_turnCount = 0;

	int m_damageToMonster = 0;
	int m_damageToPlayer = 0;

	DiceSystem::Result m_playerDice;
	DiceSystem::Result m_enemyDice;
};

