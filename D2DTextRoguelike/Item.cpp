#include "pch.h"
#include "Item.h"
#include "Player.h"

Item::Item(const std::wstring& name, ItemStatType type, int baseStatValue)
    : m_name(name), m_statType(type), m_statValue(baseStatValue), m_enhanceLevel(0)
{
}


void Item::ApplyStat(Player* pPlayer)
{
    if (pPlayer == nullptr) return;

    switch (m_statType)
    {
    case ItemStatType::Attack:
        pPlayer->attack += m_statValue;
        break;
    case ItemStatType::Defense:
        pPlayer->defense += m_statValue;
        break;
    case ItemStatType::Evasion:
        pPlayer->evasion += m_statValue;
        break;
    case ItemStatType::Hp:
        pPlayer->maxHp += m_statValue;
        break;
    }
}


void Item::Enhance(Player* pPlayer)
{
    if (pPlayer == nullptr) return;

    m_enhanceLevel++;

    int bonusStat = (m_statType == ItemStatType::Evasion) ? 1 : 3;
    m_statValue += bonusStat;

    switch (m_statType)
    {
    case ItemStatType::Attack:
        pPlayer->attack += bonusStat;
        break;
    case ItemStatType::Defense:
        pPlayer->defense += bonusStat;
        break;
    case ItemStatType::Evasion:
        pPlayer->evasion += bonusStat;
        break;
    case ItemStatType::Hp:
        pPlayer->maxHp += bonusStat;
        break;
    }
}


std::wstring Item::GetName() const { return m_name; }
ItemStatType Item::GetStatType() const { return m_statType; }
int Item::GetEnhanceLevel() const { return m_enhanceLevel; }
int Item::GetStatValue() const { return m_statValue; }