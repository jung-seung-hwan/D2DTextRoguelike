#include "pch.h"
#include "Inventory.h"
#include "Player.h"

void Inventory::EquipItem(std::unique_ptr<Item> newItem, Player* pPlayer)
{
    if (newItem == nullptr || pPlayer == nullptr) return;

    // 아이템의 스텟 타입 확인 -> 기존 장착 여부 확인을 위해
    ItemStatType type = newItem->GetStatType();

    // 아이템의 스탯을 플레이어에게 적용
    newItem->ApplyStat(pPlayer);

    // 타입 기반으로 컨테이너 탐색
    auto it = m_equipment.find(type);
    if (it != m_equipment.end())
    {
        // 기존 장비가 있다면, 기존 장비가 올려주던 스탯만큼 플레이어의 스탯을 감소시킴 (해제 효과)
        int oldStatValue = it->second->GetStatValue();
        switch (type)
        {
        case ItemStatType::Attack:  pPlayer->attack -= oldStatValue; break;
        case ItemStatType::Defense: pPlayer->defense -= oldStatValue; break;
        case ItemStatType::Evasion: pPlayer->evasion -= oldStatValue; break;
        case ItemStatType::Hp: pPlayer->maxHp -= oldStatValue; break;
        }
    }

    // 새 아이템으로 교체 (기존 아이템은 스마트 포인터에 의해 자동 소멸)
    m_equipment[type] = std::move(newItem);
}


Item* Inventory::GetRandomEquippedItem()
{
    if (m_equipment.empty()) return nullptr;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(0, static_cast<int>(m_equipment.size() - 1));
    int randomIndex = dist(gen);

    // 컨테이너의 시작지점에서 난수만큼 건너뛴 위치의 컨테이너 위치를 가져옴
    auto it = m_equipment.begin();
    std::advance(it, randomIndex);

    return it->second.get();
}


int Inventory::GetStatBonus(ItemStatType type) const
{
    // 맵에서 해당 능력치 종류를 키값으로 검색
    auto it = m_equipment.find(type);

    // 장착된 장비가 존재할 경우 내부 수치 반환
    if (it != m_equipment.end())
    {
        return it->second->GetStatValue();
    }

    // 장착된 장비가 없을 경우 0 반환
    return 0;
}


const std::map<ItemStatType, std::unique_ptr<Item>>& Inventory::GetEquipment() const
{
    return m_equipment;
}