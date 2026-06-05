#include "pch.h"
#include "DataManager.h"
#include <fstream>

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
        for (const auto& item : j["monsters"])
        {
            MonsterData data;
            // JSON 데이터 추출 및 인코딩 변환
            data.id = UTF8ToWString(item["id"].get<std::string>());
            data.name = UTF8ToWString(item["name"].get<std::string>());
            data.maxHp = item["maxHp"].get<int>();
            data.attack = item["attack"].get<int>();

            // 해시맵에 등록
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

const MonsterData* DataManager::GetMonsterData(const std::wstring& id) const
{
    auto it = m_monsterTable.find(id);
    if (it != m_monsterTable.end())
    {
        return &(it->second); // 데이터의 주소 반환
    }
    return nullptr; // 없으면 nullptr 반환
}

void DataManager::Release()
{
    m_monsterTable.clear();
}