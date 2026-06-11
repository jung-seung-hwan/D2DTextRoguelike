#include "pch.h"
#include "RoomSelectState.h"
#include "CombatState.h"
#include "EnhanceState.h"
#include "RestState.h"
#include "PlayScene.h"

#include "TextRenderer.h"
#include "ResourceManager.h"

#include "UIButton.h"
#include "UIImage.h"
#include "UIRoomButton.h"

#include "Monster.h"

void RoomSelectState::Enter(PlayScene* pScene)
{
    ID2D1Bitmap* pBgBitmap = ResourceManager::Instance().GetBitmap(L"BattleBG");
    if (pBgBitmap != nullptr)
    {
        // 화면 해상도에 맞추어 이미지 객체 생성
        auto bgImage = std::make_unique<UIImage>(pBgBitmap, EngineConfig::SCREEN_WIDTH_F, EngineConfig::SCREEN_HEIGHT_F);
        bgImage->SetLocalPosition(0.0f, 0.0f);

        // 배경화면이므로 반드시 UI 요소들 중 가장 먼저 벡터에 삽입해야 뒤로 깔림
        m_uiList.push_back(std::move(bgImage));
    }
    CreateRoomButtons(pScene);
}

void RoomSelectState::Update(PlayScene* pScene, float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void RoomSelectState::Render(PlayScene* pScene, myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    for (auto& ui : m_uiList)
    {
        ui->Render(pRenderer, pTextRenderer);
    }

    wchar_t titleBuffer[64];
    swprintf_s(titleBuffer, 64, L"현재 층: %d층\n진입할 방을 선택하십시오.", pScene->GetCurrentFloor());

    pTextRenderer->DrawText(
        titleBuffer,
        100.0f, 100.0f, 500.0f, 100.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );
}

void RoomSelectState::CreateRoomButtons(PlayScene* pScene)
{
    int currentFloor = pScene->GetCurrentFloor();

    // 1. 보스 구간 (10층, 20층, 30층)
    if (currentFloor % 10 == 0)
    {
        // 30층이면 최종 보스, 그 외(10, 20)는 중간 보스
        MonsterType type = (currentFloor == 30) ? MonsterType::Boss : MonsterType::MidBoss;
        std::wstring btnText = (type == MonsterType::Boss) ? L"최종 보스방 진입" : L"중간 보스방 진입";

        // 보스방 이미지 로드 및 UIRoomButton 생성
        ID2D1Bitmap* pDoorImg = ResourceManager::Instance().GetBitmap(L"BossDr");
        auto roomBtn = std::make_unique<UIRoomButton>(pDoorImg, btnText, 300.0f, 300.0f);

        roomBtn->SetLocalPosition(EngineConfig::SCREEN_CENTER_X - 150.0f, 300.0f);
        roomBtn->SetOnClick([this, pScene, type, currentFloor]()
            {
                pScene->ChangeState(std::make_unique<CombatState>(currentFloor, type));
            });

        m_uiList.push_back(std::move(roomBtn));
    }
    // 2. 휴식 구간 (9층, 19층, 29층)
    else if (currentFloor % 10 == 9)
    {
        ID2D1Bitmap* pDoorImg = ResourceManager::Instance().GetBitmap(L"BreakDr");
        auto roomBtn = std::make_unique<UIRoomButton>(pDoorImg, L"휴식방", 300.0f, 300.0f);

        roomBtn->SetLocalPosition(EngineConfig::SCREEN_CENTER_X - 150.0f, 300.0f);
        roomBtn->SetOnClick([this, pScene, currentFloor]()
        {
            pScene->ChangeState(std::make_unique<RestState>(currentFloor));
        });

        m_uiList.push_back(std::move(roomBtn));
    }
    // 3. 일반 구간 (1~8층 패턴)
    else
    {
        float startX = EngineConfig::SCREEN_CENTER_X - 550.0f;
        float yPos = 300.0f;

        // C++11 표준 메르센 트위스터 난수 엔진 초기화
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(1, 100);

        for (int i = 0; i < 3; ++i)
        {
            // 가중치 적용: 1~70이 나오면 전투방(70%), 71~100이 나오면 강화방(30%)
            int roll = dist(gen);
            bool isCombatRoom = (roll <= 70);

            std::wstring btnText = isCombatRoom ? L"전투방" : L"강화방";
            // 상태에 따라 로드할 이미지 키값 분기
            std::wstring imgKey = isCombatRoom ? L"BattleDr" : L"AwardDr";
            ID2D1Bitmap* pDoorImg = ResourceManager::Instance().GetBitmap(imgKey);

            auto roomBtn = std::make_unique<UIRoomButton>(pDoorImg, btnText, 300.0f, 300.0f);
            roomBtn->SetLocalPosition(startX + (i * 400.0f), yPos);

            roomBtn->SetOnClick([this, pScene, isCombatRoom, currentFloor]()
                {
                    if (isCombatRoom)
                    {
                        pScene->ChangeState(std::make_unique<CombatState>(currentFloor, MonsterType::Normal));
                    }
                    else
                    {
                        pScene->ChangeState(std::make_unique<EnhanceState>(currentFloor));
                    }
                });

            m_uiList.push_back(std::move(roomBtn));
        }
    }
}

void RoomSelectState::Exit(PlayScene* pScene)
{
    m_uiList.clear();
}