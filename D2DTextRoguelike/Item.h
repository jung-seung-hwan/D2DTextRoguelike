#pragma once
#include <string>

class Player;

// 아이템 스텟 종류
enum class ItemStatType
{
    Attack,
    Defense,
    Evasion
};

class Item
{
public:
    Item(const std::wstring& name, ItemStatType type, int baseStatValue);
    virtual ~Item() = default;

    // 획득 시 플레이어 스탯 증가
    // 기존 플레이어 스텟에 장비의 스텟을 더함
    void ApplyStat(Player* pPlayer);

    // 강화 단계에 따라 장비 스텟 성장
    // 회피율은 1, 공격력과 방어력은 3
    void Enhance(Player* pPlayer);

    // 읽기 전용 변수들
    std::wstring GetName() const;
    ItemStatType GetStatType() const;
    int GetEnhanceLevel() const;
    int GetStatValue() const;       // 장비 교체 시 기존 수치 차감

private:
    std::wstring m_name;
    ItemStatType m_statType;
    int m_statValue;     // 아이템 자체의 스탯
    int m_enhanceLevel;  // 강화 단계
};