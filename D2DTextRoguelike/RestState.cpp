#include "pch.h"
#include "RestState.h"
#include "RoomSelectState.h" 
#include "PlayScene.h"

#include "D2DRender.h"
#include "TextRenderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"

#include "UIObject.h"
#include "DialoguePanel.h"
#include "UIbutton.h"
#include "UIImage.h"
#include "StatusPanel.h"

RestState::RestState(int floor)
    : m_floor(floor)
{
}

void RestState::Enter(PlayScene* pScene)
{
    // 플레이어 상태창 생성 (HP 등)
    auto playerStatus = std::make_unique<StatusPanel>(
        pScene->GetPlayer(),
        300.0f,
        10.0f
    );

    // 플레이어 상태창을 UI 목록에 등록
    playerStatus->SetLocalPosition(520.0f, 400.0f);
    m_uiList.push_back(std::move(playerStatus));


    m_player = pScene->GetPlayer();

    CreateUI(pScene);

    if (m_dialoguePanel)
    {
        wchar_t buffer[256];
        swprintf_s(buffer, 256, L"- %d층 -\n휴식 방에 입장하였습니다.", m_floor);
        m_dialoguePanel->PlayText(buffer);
        m_dialoguePanel->SetActive(true);
    }
}


void RestState::Update(PlayScene* pScene, float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void RestState::Render(PlayScene* pScene, myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer)
{
    ID2D1Bitmap* pBgBitmap = nullptr;
    pBgBitmap = ResourceManager::Instance().GetBitmap(L"BreakBG");

    if (pBgBitmap != nullptr)
    {
        m_pRenderer->DrawBitmap(pBgBitmap, D2D1::RectF(0.0f, 0.0f, EngineConfig::SCREEN_WIDTH_F, EngineConfig::SCREEN_HEIGHT_F));
    }

    ID2D1Bitmap* playerBitmap = ResourceManager::Instance().GetBitmap(L"Player");

    if (playerBitmap != nullptr)
    {
        // 밑에서 50.0f, 50.0f 띄우고 400x400 크기로 배치
        m_pRenderer->DrawBitmap(
            playerBitmap,
            D2D1::RectF(50.0f, 250.0f, 450.0f, 650.0f) // (left, top, right, bottom)
        );
    }

    wchar_t floorBuffer[64];
    swprintf_s(floorBuffer, 64, L"현재 층: %d층", m_floor);
    pTextRenderer->DrawText(
        floorBuffer,
        100.0f, 60.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::Yellow)
    );


    // UI 일괄 렌더링
    for (auto& ui : m_uiList)
    {
        ui->Render(m_pRenderer, pTextRenderer);
    }
}

void RestState::CreateUI(PlayScene* pScene)
{
    // 대화창 생성 로직
    auto dialoguePanel = std::make_unique<DialoguePanel>(860.0f, 150.0f);
    m_dialoguePanel = dialoguePanel.get(); // 제어용 포인터 저장
    m_dialoguePanel->SetLocalPosition(20.0f, 530.0f);
    m_dialoguePanel->SetActive(false);
    m_uiList.push_back(std::move(dialoguePanel));

    // 상태를 넘기는 다음 버튼
    auto nextBtn = std::make_unique<UIButton>(L"다음 층으로", 120.0f, 40.0f);
    UIButton* pNextBtn = nextBtn.get(); // 원시 포인터 보관
    pNextBtn->SetLocalPosition(300.0f, 470.0f);
    pNextBtn->SetActive(false);
    pNextBtn->SetOnClick([this, pScene]()
        {
            pScene->IncreaseFloor();
            pScene->ChangeState(std::make_unique<RoomSelectState>());
        });

    // 보상을 받는 버튼
    auto rewardBtn = std::make_unique<UIButton>(L"보상 받기", 120.0f, 40.0f);
    UIButton* pRewardBtn = rewardBtn.get();
    pRewardBtn->SetLocalPosition(160.0f, 470.0f);
    pRewardBtn->SetOnClick([this, pNextBtn, pRewardBtn]()
        {
            // 보상 지급 로직 (예: 체력 회복 등)
            m_player->hp = m_player->maxHp;

            if (m_dialoguePanel)
            {
                wchar_t buffer[256];
                swprintf_s(buffer, 256, L"체력을 모두 회복했다!");
                m_dialoguePanel->PlayText(buffer);
            }

            // 보상 버튼은 숨기고 다음 층 이동 버튼 표시
            pRewardBtn->SetActive(false);
            pNextBtn->SetActive(true);
        });

    m_uiList.push_back(std::move(nextBtn));
    m_uiList.push_back(std::move(rewardBtn));
}

void RestState::Exit(PlayScene* pScene)
{
    m_uiList.clear();
    m_dialoguePanel = nullptr;
}