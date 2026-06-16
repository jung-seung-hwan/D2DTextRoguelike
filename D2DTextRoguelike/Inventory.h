#pragma once
#include <vector>
#include <memory>
#include "Item.h"

class Player;

class Inventory
{
public:
    Inventory() = default;
    ~Inventory() = default;

    // 아이템 획득 (플레이어 포인터를 함께 넘겨 스탯 적용)
    void EquipItem(std::unique_ptr<Item> newItem, Player* pPlayer);

    // 랜덤한 장비 강화을 위해 보유한 장비 중 랜덤한 장비를 반환하는 함수
    Item* GetRandomEquippedItem();

    // 플레이어 스텟이 적용된 장비의 능력치 반환 함수
    int GetStatBonus(ItemStatType type) const;

    // 전체 아이템 정보를 반환
    const std::map<ItemStatType, std::unique_ptr<Item>>& GetEquipment() const;

    // 인벤토리 초기화
    void Clear();

private:
    // 능력치 별로 하나의 아이템만 가질수 있도록 구조 설정
    std::map<ItemStatType, std::unique_ptr<Item>> m_equipment;
};