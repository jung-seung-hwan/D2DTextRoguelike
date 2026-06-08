#include "pch.h"
#include "RoomSelectState.h"
#include "CombatState.h"
#include "DataManager.h"

void RoomSelectState::Enter(PlayScene* pScene)
{
    CreateRoomButtons(pScene);
}

void RoomSelectState::Update(PlayScene* pScene, float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void RoomSelectState::Render(PlayScene* pScene, ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    wchar_t titleBuffer[64];
    swprintf_s(titleBuffer, 64, L"현재 층: %d층\n진입할 방을 선택하십시오.", pScene->GetCurrentFloor());

    pTextRenderer->DrawText(
        titleBuffer,
        100.0f, 100.0f, 500.0f, 100.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );

    for (auto& ui : m_uiList)
    {
        ui->Render(pContext, pTextRenderer);
    }
}

void RoomSelectState::CreateRoomButtons(PlayScene* pScene)
{
    // 3개의 방 선택지 생성 (가로 배열)
    float startX = 100.0f;
    float yPos = 300.0f;

    for (int i = 0; i < 3; ++i)
    {
        // 임시로 모든 방을 전투 방으로 가정
        auto roomBtn = std::make_unique<UIButton>(L"미지의 방", 150.0f, 80.0f);
        roomBtn->SetLocalPosition(startX + (i * 200.0f), yPos);

        // 버튼 클릭 시의 상태 전환 로직 (람다 캡처를 통한 씬 참조)
        roomBtn->SetOnClick([this, pScene]()
            {
                int currentFloor = pScene->GetCurrentFloor();

                // 층수 진행에 따른 보스 난이도 분기
                MonsterType type = MonsterType::Normal;
                if (currentFloor % 10 == 0) type = MonsterType::Boss;
                else if (currentFloor % 5 == 0) type = MonsterType::MidBoss;

                // CombatState로 제어권 및 소유권 전환
                pScene->ChangeState(std::make_unique<CombatState>(currentFloor, type));
            });

        m_uiList.push_back(std::move(roomBtn));
    }
}

void RoomSelectState::Exit(PlayScene* pScene)
{
    m_uiList.clear();
}