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
        360.0f,
        12.0f
    );

    // 플레이어 상태창을 UI 목록에 등록
    playerStatus->SetLocalPosition(130.0f, 330.0f);
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
            D2D1::RectF(70.0f, 360.0f, 560.0f, 850.0f) // (left, top, right, bottom)
        );
    }

    wchar_t floorBuffer[64];
    swprintf_s(floorBuffer, 64, L"현재 층: %d층", m_floor);
    pTextRenderer->DrawText(
        floorBuffer,
        EngineConfig::SCREEN_WIDTH_F - 400.0f, 40.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::Yellow),
        TextAlign::Right
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
    auto dialoguePanel = std::make_unique<DialoguePanel>(1160.0f, 170.0f);
    m_dialoguePanel = dialoguePanel.get(); // 제어용 포인터 저장
    m_dialoguePanel->SetLocalPosition(60.0f, 800.0f);
    m_dialoguePanel->SetActive(false);
    m_uiList.push_back(std::move(dialoguePanel));

    // 상태를 넘기는 다음 버튼
    auto nextBtn = std::make_unique<UIButton>(L"다음 층으로", 120.0f, 40.0f);
    UIButton* pNextBtn = nextBtn.get();
    pNextBtn->SetLocalPosition(1080.0f, 752.0f);
    pNextBtn->SetActive(false);
    pNextBtn->SetOnClick([this, pScene]()
        {
            pScene->IncreaseFloor();
            pScene->ChangeState(std::make_unique<RoomSelectState>());
        });

    // 보상을 받는 버튼
    auto enhanceBtn = std::make_unique<UIButton>(L"장비 강화", 120.0f, 40.0f);
    UIButton* pEnhanceBtn = enhanceBtn.get();
    pEnhanceBtn->SetLocalPosition(820.0f, 752.0f);

    // 보상 선택 버튼 2: 체력 회복
    auto healBtn = std::make_unique<UIButton>(L"체력 회복", 120.0f, 40.0f);
    UIButton* pHealBtn = healBtn.get();
    pHealBtn->SetLocalPosition(950.0f, 752.0f);

    pEnhanceBtn->SetOnClick([this, pNextBtn, pEnhanceBtn, pHealBtn]()
        {
            Item* targetItem = m_player->GetInventory().GetRandomEquippedItem();

            if (m_dialoguePanel)
            {
                wchar_t buffer[256] = { 0, };

                if (targetItem != nullptr)
                {
                    targetItem->Enhance(m_player);
                    swprintf_s(buffer, 256, L"신비한 힘이 장비에 스며듭니다.\n%s 장비가 강화되었다! (현재 강화: +%d)",
                        targetItem->GetName().c_str(),
                        targetItem->GetEnhanceLevel());
                }
                else
                {
                    swprintf_s(buffer, 256, L"장착 중인 장비가 없어 강화에 실패했습니다...");
                }

                m_dialoguePanel->PlayText(buffer);
            }

            // 보상 획득 완료 처리 (두 선택지 모두 숨김)
            pEnhanceBtn->SetActive(false);
            pHealBtn->SetActive(false);
            pNextBtn->SetActive(true);
        });

    pHealBtn->SetOnClick([this, pNextBtn, pEnhanceBtn, pHealBtn]()
        {
            m_player->hp = m_player->maxHp;

            if (m_dialoguePanel)
            {
                wchar_t buffer[256];
                swprintf_s(buffer, 256, L"휴식을 취합니다.\n체력을 모두 회복했습니다!");
                m_dialoguePanel->PlayText(buffer);
            }

            // 보상 획득 완료 처리 (두 선택지 모두 숨김)
            pEnhanceBtn->SetActive(false);
            pHealBtn->SetActive(false);
            pNextBtn->SetActive(true);
        });


    m_uiList.push_back(std::move(nextBtn));
    m_uiList.push_back(std::move(enhanceBtn));
    m_uiList.push_back(std::move(healBtn));
}

void EnhanceState::Exit(PlayScene* pScene)
{
    m_uiList.clear();
    m_dialoguePanel = nullptr;
}