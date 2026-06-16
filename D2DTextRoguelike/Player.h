#pragma once

#include <string>
#include "GameObject.h"
#include "Inventory.h"

class Player : public GameObject
{
public:
    Player(); // 생성자 선언
    virtual ~Player() = default;

    // 기존 재화 및 특수 상태 변수
    int gold = 0;
    int shield = 0;

    // 플레이어 객체 초기화용 함수
    void Reset();
    void Heal(int amount);

    // 장비 정보 반환을 위한 함수
    Inventory& GetInventory() { return m_inventory; }

private:
    Inventory m_inventory;
};