#include "pch.h"
#include "PlayScene.h"
#include "DataManager.h"
#include "InputManager.h"
#include "ResourceManager.h"

void PlayScene::Initialize()
{
    // 플레이어 임시 초기화
    m_player.name = L"플레이어";
    m_player.maxHp = 100;
    m_player.hp = 100;
    m_player.attack = 10;
    m_player.defense = 0;
    m_player.evasion = 10;
    m_player.isDead = false;

    StartBattle(L"MOB_SLIME");

    CreateUI();

}

void PlayScene::Update(float deltaTime)
{
    // UI 일괄 갱신
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void PlayScene::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    ID2D1Bitmap* playerBitmap = ResourceManager::Instance().GetBitmap(L"PlayerDummy");

    if (playerBitmap != nullptr)
    {
        pContext->DrawBitmap(
            playerBitmap,
            D2D1::RectF(0.0f, 0.0f, 700.0f, 1000.0f)
        );
    }

    ID2D1Bitmap* slimeBitmap = ResourceManager::Instance().GetBitmap(L"SlimeDummy");

    if (slimeBitmap != nullptr)
    {
        pContext->DrawBitmap(
            slimeBitmap,
            D2D1::RectF(560.0f, 130.0f, 720.0f, 290.0f)
        );
    }

    // 화면 출력 테스트용 텍스트
    pTextRenderer->DrawText(
        L"Play Started!",
        100.0f, 100.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::Red)
    );

    wchar_t floorBuffer[64];
    swprintf_s(floorBuffer, 64, L"현재 층: %d층", m_currentFloor);

    pTextRenderer->DrawText(
        floorBuffer,
        100.0f, 60.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::Yellow)
    );

    if (m_currentMonster != nullptr)
    {
        wchar_t buffer[256];
        swprintf_s(buffer, 256, L"이름: %s\n체력: %d / %d\n공격력: %d",
            m_monster.name.c_str(),
            m_monster.hp,
            m_monster.maxHp,
            m_monster.attack);


        pTextRenderer->DrawText(
            buffer,
            100.0f, 150.0f, 300.0f, 150.0f, // 텍스트가 그려질 사각형 영역
            D2D1::ColorF(D2D1::ColorF::White)
        );
    }
    wchar_t playerBuffer[256];
    swprintf_s(playerBuffer, 256, L"플레이어\n체력: %d / %d\n공격력: %d",
        m_player.hp,
        m_player.maxHp,
        m_player.attack
    );

    pTextRenderer->DrawText(
        playerBuffer,
        100.0f, 320.0f, 300.0f, 150.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );
    

    // UI 일괄 렌더링
    for (auto& ui : m_uiList)
    {
        ui->Render(pContext, pTextRenderer);
    }
}

void PlayScene::Release()
{
    m_currentMonster = nullptr;
    m_dialoguePanel = nullptr;
    m_uiList.clear();
}


void PlayScene::StartBattle(const std::wstring& monsterId)
{
    m_currentMonster = DataManager::Instance().GetMonsterData(monsterId);

    if (m_currentMonster == nullptr)
        return;

    m_monster.InitFromData(*m_currentMonster);
    m_combatManager.StartBattle(&m_player, &m_monster);
}

void PlayScene::GoNextFloor()
{
    m_currentFloor++;

    StartBattle(L"MOB_SLIME");

    if (m_dialoguePanel != nullptr)
    {
        wchar_t buffer[128];
        swprintf_s(buffer, 128, L"- %d층 -", m_currentFloor);
        m_dialoguePanel->PlayText(buffer);
    }
}

void PlayScene::CreateUI()
{
    CreateDialoguePanel();


    CreateButton(L"공격", 160.0f, 470.0f, [this]()
        {
            if (m_combatManager.IsBattleEnd())
                return;

            m_combatManager.PlayerAction(PLAYERACTION::ATTACK);

            if (m_dialoguePanel == nullptr)
                return;

            wchar_t buffer[256];

            if (m_combatManager.GetState() == BATTLESTATE::VICTORY)
            {
                swprintf_s(buffer, 256, L"%s을/를 쓰러뜨렸다!", m_monster.name.c_str());
            }
            else if (m_combatManager.GetState() == BATTLESTATE::DEFEAT)
            {
                swprintf_s(buffer, 256, L"플레이어가 쓰러졌다...");
            }
            else
            {
                swprintf_s(
                    buffer,
                    256,
                    L"플레이어 공격!\n%s에게 %d 대미지!\n%s의 반격!\n플레이어가 %d 대미지를 받았다.",
                    m_monster.name.c_str(),
                    m_combatManager.GetDamageToMonster(),
                    m_monster.name.c_str(),
                    m_combatManager.GetDamageToPlayer()
                );
            }

            m_dialoguePanel->PlayText(buffer);
        });

    CreateButton(L"다음 층", 300.0f, 470.0f, [this]()
        {
            if (m_combatManager.GetState() == BATTLESTATE::VICTORY)
            {
                GoNextFloor();
            }
        });
}

void PlayScene::CreateDialoguePanel()
{
    auto dialoguePanel = std::make_unique<DialoguePanel>(860.0f, 150.0f);

    m_dialoguePanel = dialoguePanel.get();
    m_dialoguePanel->SetLocalPosition(20.0f, 530.0f);
    m_dialoguePanel->SetActive(false);

    m_uiList.push_back(std::move(dialoguePanel));
}

void PlayScene::CreateButton(const std::wstring& text, float x, float y, std::function<void()> onClick)
{
    auto button = std::make_unique<UIButton>(text, 120.0f, 40.0f);
    button->SetLocalPosition(x, y);
    button->SetOnClick(onClick);

    m_uiList.push_back(std::move(button));
}

