#pragma once
#include "SingletonBase.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <random>

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

class DataManager : public SingletonBase<DataManager>
{
    friend class SingletonBase<DataManager>;

public:
    // JSON 파일을 읽어 해시맵에 적재하는 함수
    bool LoadMonsterData(const std::string& filePath);

    // 게임 로직에서 ID로 데이터를 빠르게(O(1)) 찾아오는 함수
    const MonsterData* GetMonsterData(const std::wstring& id) const;

    // 조건에 맞는 무작위 몬스터 반환 함수
    const MonsterData* GetRandomMonster(int floor, MonsterType type);

    void Release();

private:
    DataManager() : m_rng(m_rd()) {}
    ~DataManager() { Release(); }

    // 데이터 보관 장부 (Key: 몬스터 ID, Value: 실제 데이터)
    std::unordered_map<std::wstring, MonsterData> m_monsterTable;

    // 모던 C++ 난수 생성기 멤버 변수
    std::random_device m_rd;
    std::mt19937 m_rng;
};