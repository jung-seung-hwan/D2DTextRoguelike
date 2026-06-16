#include "pch.h"
#include "EnhanceState.h"
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

EnhanceState::EnhanceState(int floor)
    : m_floor(floor)
{
}

void EnhanceState::Enter(PlayScene* pScene)
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
        swprintf_s(buffer, 256, L"- %d층 -\n강화 방에 입장하였습니다.", m_floor);
        m_dialoguePanel->PlayText(buffer);
        m_dialoguePanel->SetActive(true);
    }
}


void EnhanceState::Update(PlayScene* pScene, float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void EnhanceState::Render(PlayScene* pScene, myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer)
{
    ID2D1Bitmap* pBgBitmap = nullptr;
    pBgBitmap = ResourceManager::Instance().GetBitmap(L"AwardBG");

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

void EnhanceState::CreateUI(PlayScene* pScene)
{
    // 대화창 생성 로직
    auto dialoguePanel = std::make_unique<DialoguePanel>(860.0f, 150.0f);
    m_dialoguePanel = dialoguePanel.get(); // 제어용 포인터 저장
    m_dialoguePanel->SetLocalPosition(20.0f, 530.0f);
    m_dialoguePanel->SetActive(false);
    m_uiList.push_back(std::move(dialoguePanel));

    // 상태를 넘기는 다음 버튼
    auto nextBtn = std::make_unique<UIButton>(L"다음 층으로", 120.0f, 40.0f);
    UIButton* pNextBtn = nextBtn.get();
    pNextBtn->SetLocalPosition(300.0f, 470.0f);
    pNextBtn->SetActive(false);
    pNextBtn->SetOnClick([this, pScene]()
        {
            pScene->IncreaseFloor();
            pScene->ChangeState(std::make_unique<RoomSelectState>());
        });

    // 보상을 받는 버튼
    auto rewardBtn = std::make_unique<UIButton>(L"장비 강화", 120.0f, 40.0f);
    UIButton* pRewardBtn = rewardBtn.get();
    pRewardBtn->SetLocalPosition(160.0f, 470.0f);
    pRewardBtn->SetOnClick([this, pNextBtn, pRewardBtn]()
        {
            // 인벤토리에서 무작위 장착 장비 획득
            Item* targetItem = m_player->GetInventory().GetRandomEquippedItem();

            if (m_dialoguePanel)
            {
                wchar_t buffer[256] = { 0, };

                if (targetItem != nullptr)
                {
                    // 장비가 존재할 경우: 강화 로직 실행
                    targetItem->Enhance(m_player);

                    swprintf_s(buffer, 256, L"신비한 힘이 장비에 스며듭니다.\n%s 장비가 강화되었다! (현재 강화: +%d)",
                        targetItem->GetName().c_str(),
                        targetItem->GetEnhanceLevel());
                }
                else
                {
                    // 장착 중인 장비가 없을 경우: 대체 보상 (체력 회복)
                    m_player->hp = m_player->maxHp;
                    swprintf_s(buffer, 256, L"장착 중인 장비가 없습니다.\n대신 신비한 힘이 상처를 치유하여 체력을 모두 회복했다!");
                }

                m_dialoguePanel->PlayText(buffer);
            }

            // 보상 버튼 비활성화 및 다음 층 이동 버튼 활성화
            pRewardBtn->SetActive(false);
            pNextBtn->SetActive(true);
        });

    m_uiList.push_back(std::move(nextBtn));
    m_uiList.push_back(std::move(rewardBtn));
}

void EnhanceState::Exit(PlayScene* pScene)
{
    m_uiList.clear();
    m_dialoguePanel = nullptr;
}