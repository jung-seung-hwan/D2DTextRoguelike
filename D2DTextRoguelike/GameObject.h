#pragma once
#include <string>

class GameObject
{
public:
	std::wstring name;

	int maxHp = 0;
	int hp = 0;

	int attack = 0;
	int defense = 0;
	int evasion = 0;

	bool isDead = false;

public:
	virtual ~GameObject() = default;

	virtual void Init() {}
	virtual void Update(float deltaTime) {}
	virtual void Render() {}

	virtual int TakeDamage(int damage);
	virtual int Attack(GameObject& target);
	//virtual void Defend();

	bool IsDead() const;
};