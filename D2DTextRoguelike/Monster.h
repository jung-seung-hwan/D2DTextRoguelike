#pragma once

#include "GameObject.h"
#include "DataManager.h"

class Monster : public GameObject
{
public:
    std::wstring monsterId;

public:
    void InitFromData(const MonsterData& data);
};