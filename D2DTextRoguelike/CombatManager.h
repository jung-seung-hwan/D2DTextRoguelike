#pragma once

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
private:
	Player* m_player = nullptr;
	Monster* m_monster = nullptr;

	BATTLESTATE m_state = BATTLESTATE::NONE;
	int m_turnCount = 0;

public:
	void StartBattle(Player* player, Monster* monster);

	void PlayerAction(PLAYERACTION action);

	void EnemyTurn();

	bool IsBattleEnd() const;
	BATTLESTATE GetState() const;
};

