#pragma once
#include "SingletonBase.h"
#include <string>
#include <unordered_map>

// JSON에서 읽어들일 몬스터 데이터 구조체
struct MonsterData
{
    std::wstring id;
    std::wstring name;
    int maxHp;
    int attack;
};

class DataManager : public SingletonBase<DataManager>
{
    friend class SingletonBase<DataManager>;

public:
    // JSON 파일을 읽어 해시맵에 적재하는 함수
    bool LoadMonsterData(const std::string& filePath);

    // 게임 로직에서 ID로 데이터를 빠르게(O(1)) 찾아오는 함수
    const MonsterData* GetMonsterData(const std::wstring& id) const;

    void Release();

private:
    DataManager() = default;
    ~DataManager() { Release(); }

    // 데이터 보관 장부 (Key: 몬스터 ID, Value: 실제 데이터)
    std::unordered_map<std::wstring, MonsterData> m_monsterTable;
};