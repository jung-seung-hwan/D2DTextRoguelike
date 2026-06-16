#include "pch.h"
#include "DataManager.h"

using json = nlohmann::json;

// UTF-8을 std::wstring으로 변환하는 유틸리티 함수
static std::wstring UTF8ToWString(const std::string& utf8Str)
{
    if (utf8Str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

bool DataManager::LoadMonsterData(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        OutputDebugStringW(L"[Error] Failed to open JSON file.\n");
        return false;
    }

    try
    {
        json j;
        file >> j; // JSON 파일 읽기 완료

        // JSON 배열 순회
        for (const auto& item : j["Monsters"])
    {
        MonsterData data;

        // 기본 식별자 파싱 및 문자열 변환
        data.id = UTF8ToWString(item["Id"].get<std::string>());
        data.name = UTF8ToWString(item["Name"].get<std::string>());

        // 등급 파싱
        std::string typeStr = item["Type"].get<std::string>();
        if (typeStr == "Boss")
        {
            data.type = MonsterType::Boss;
        }
        else if (typeStr == "MidBoss")
        {
            data.type = MonsterType::MidBoss;
        }
        else
        {
            data.type = MonsterType::Normal;
        }

        // 전투 및 스폰 속성 파싱
        data.minFloor = item["MinFloor"].get<int>();
        data.maxFloor = item["MaxFloor"].get<int>();
        data.maxHp = item["MaxHp"].get<int>();
        data.attack = item["Attack"].get<int>();
        data.evasion = item["Evasion"].get<int>();

        // 해시맵에 적재
        m_monsterTable[data.id] = data;
    }

        OutputDebugStringW(L"[System] MonsterData Loaded Successfully.\n");
        return true;
    }
    catch (const json::exception& e)
    {
        OutputDebugStringA(e.what()); // 파싱 에러 출력
        return false;
    }
}

const MonsterData* DataManager::GetRandomMonster(int floor, MonsterType type)
{
    std::vector<const MonsterData*> validPool;

    // 전체 테이블 순회 및 조건 필터링
    for (const auto& pair : m_monsterTable)
    {
        const MonsterData& data = pair.second;

        // 요청된 등급과 층수 조건에 부합하는지 검사
        if (data.type == type && floor >= data.minFloor && floor <= data.maxFloor)
        {
            validPool.push_back(&data);
        }
    }

    // 예외 처리
    if (validPool.empty())
    {
        // 조건에 맞는 몬스터가 JSON에 정의되어 있지 않은 경우
        return nullptr;
    }

    // 난수 생성 및 무작위 데이터 반환
    std::uniform_int_distribution<size_t> dist(0, validPool.size() - 1);
    size_t randomIndex = dist(m_rng);

    return validPool[randomIndex];
}

const MonsterData* DataManager::GetMonsterData(const std::wstring& id) const
{
    auto it = m_monsterTable.find(id);
    if (it != m_monsterTable.end())
    {
        return &(it->second); // 데이터의 주소 반환
    }
    return nullptr; // 없으면 nullptr 반환
}


bool DataManager::LoadItemData(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        OutputDebugStringW(L"[Error] Failed to open Item JSON file.\n");
        return false;
    }

    try
    {
        json j;
        file >> j; // JSON 파일 읽기 완료

        // JSON 배열 순회 ("items" 키 기준)
        for (const auto& item : j["items"])
        {
            ItemData data;

            // 기본 식별자 파싱 및 문자열 변환
            data.id = item["id"].get<int>();
            data.name = UTF8ToWString(item["name"].get<std::string>());

            // 능력치 타입 파싱 및 열거형 변환
            std::string statStr = item["statType"].get<std::string>();
            if (statStr == "Attack")
            {
                data.statType = ItemStatType::Attack;
            }
            else if (statStr == "Defense")
            {
                data.statType = ItemStatType::Defense;
            }
            else if (statStr == "Evasion")
            {
                data.statType = ItemStatType::Evasion;
            }

            // 수치 및 등장 층수 파싱
            data.baseValue = item["baseValue"].get<int>();
            data.minFloor = item["minFloor"].get<int>();
            data.maxFloor = item["maxFloor"].get<int>();

            // 벡터에 적재
            m_itemDatas.push_back(data);
        }

        OutputDebugStringW(L"[System] ItemData Loaded Successfully.\n");
        return true;
    }
    catch (const json::exception& e)
    {
        OutputDebugStringA(e.what()); // 파싱 에러 출력
        return false;
    }
}


const ItemData* DataManager::GetRandomItemData(int currentFloor) const
{
    std::vector<const ItemData*> availableItems;

    // 현재 층수에 등장 가능한 아이템만 필터링
    for (const auto& data : m_itemDatas)
    {
        if (currentFloor >= data.minFloor && currentFloor <= data.maxFloor)
        {
            availableItems.push_back(&data);
        }
    }

    // 필터링된 아이템이 없다면 예외 처리
    if (availableItems.empty()) return nullptr;

    // 필터링된 목록 중 무작위 선택
    return availableItems[std::uniform_int_distribution<int>(0, (int)availableItems.size() - 1)(m_rng)];
}

void DataManager::Release()
{
    m_monsterTable.clear();
    m_itemDatas.clear();
}

