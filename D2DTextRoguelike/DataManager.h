#pragma once
#include "SingletonBase.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include "Item.h"

enum class MonsterType
{
    Normal,
    MidBoss,
    Boss
};

struct MonsterData
{
    std::wstring id;
    std::wstring name;
    MonsterType type;
    int minFloor;
    int maxFloor;
    int maxHp;
    int attack;
    int evasion;
};

struct ItemData
{
    int id;
    std::wstring name;
    ItemStatType statType;
    int baseValue;
    int minFloor;
    int maxFloor;
};

class DataManager : public SingletonBase<DataManager>
{
    friend class SingletonBase<DataManager>;

public:
    // JSON 파일을 읽어 해시맵에 적재
    bool LoadMonsterData(const std::string& filePath);
    bool LoadItemData(const std::string& filePath);

    const MonsterData* GetMonsterData(const std::wstring& id) const;

    // 조건에 맞는 무작위 몬스터 반환
    const MonsterData* GetRandomMonster(int floor, MonsterType type);

    // 현재 층수에 맞는 아이템 데이터 중 하나를 무작위로 반환
    const ItemData* GetRandomItemData(int currentFloor) const;

    void Release();

private:
    DataManager() : m_rng(m_rd()) {}
    ~DataManager() { Release(); }

    // 데이터 보관 장부 (Key: 몬스터 ID, Value: 실제 데이터)
    std::unordered_map<std::wstring, MonsterData> m_monsterTable;
    std::vector<ItemData> m_itemDatas;

    std::random_device m_rd;
    mutable std::mt19937 m_rng;
};