#include "pch.h"
#include "PlayScene.h"
#include "DataManager.h"

void PlayScene::Initialize()
{
    // 몬스터 생성, 플레이어 초기화 등
    m_currentMonster = DataManager::Instance().GetMonsterData(L"MOB_SLIME");
}

void PlayScene::Update(float deltaTime)
{
    // 전투 로직, 턴 계산, 몬스터 AI 갱신 등
}

void PlayScene::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    // 전투 화면 출력 테스트용 텍스트
    pTextRenderer->DrawTextW(
        L"Play Started!",
        100.0f, 100.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::Red)
    );

    if (m_currentMonster != nullptr)
    {
        wchar_t buffer[256];
        swprintf_s(buffer, 256, L"이름: %s\n체력: %d\n공격력: %d",
            m_currentMonster->name.c_str(),
            m_currentMonster->maxHp,
            m_currentMonster->attack);

        pTextRenderer->DrawTextW(
            buffer,
            100.0f, 150.0f, 300.0f, 150.0f, // 텍스트가 그려질 사각형 영역
            D2D1::ColorF(D2D1::ColorF::White)
        );
    }
}

void PlayScene::Release()
{
    m_currentMonster = nullptr;
}