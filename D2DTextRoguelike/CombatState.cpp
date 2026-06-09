#include "pch.h"
#include "CombatState.h"
#include "DataManager.h"
#include "ResourceManager.h"
#include "RoomSelectState.h" 
#include "StatusPanel.h"
#include "SceneManager.h"

CombatState::CombatState(int floor, MonsterType type)
    : m_floor(floor), m_type(type)
{
}

void CombatState::Enter(PlayScene* pScene)
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

    StartCombat(pScene);
    CreateUI(pScene);

    if (m_dialoguePanel)
    {
        wchar_t buffer[256];
        swprintf_s(buffer, 256, L"- %d층 -\n야생의 %s(이)가 나타났다!", m_floor, m_monster.name.c_str());
        m_dialoguePanel->PlayText(buffer);
        m_dialoguePanel->SetActive(true);
    }
}

void CombatState::StartCombat(PlayScene* pScene)
{
    const MonsterData* data = DataManager::Instance().GetRandomMonster(m_floor, m_type);
    if (!data) return;

    m_monster.name = data->name;
    m_monster.maxHp = data->maxHp;
    m_monster.hp = data->maxHp;
    m_monster.attack = data->attack;
    m_monster.evasion = data->evasion;
    m_monster.isDead = false;

    // PlayScene이 소유한 플레이어 포인터를 받아 전투 시작
    m_combatManager.StartBattle(pScene->GetPlayer(), &m_monster);
}

void CombatState::Update(PlayScene* pScene, float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void CombatState::Render(PlayScene* pScene, myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer)
{

    ID2D1Bitmap* playerBitmap = ResourceManager::Instance().GetBitmap(L"Player");

    if (playerBitmap != nullptr)
    {
        // 밑에서 50.0f, 50.0f 띄우고 400x400 크기로 배치
        m_pRenderer->DrawBitmap(
            playerBitmap,
            D2D1::RectF(50.0f, 250.0f, 450.0f, 650.0f) // (left, top, right, bottom)
        );
    }

    ID2D1Bitmap* slimeBitmap = ResourceManager::Instance().GetBitmap(L"Slime");

    if (slimeBitmap != nullptr)
    {
        m_pRenderer->DrawBitmap(
            slimeBitmap,
            D2D1::RectF(560.0f, 130.0f, 720.0f, 290.0f) // (left, top, right, bottom)
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

void CombatState::CreateUI(PlayScene* pScene)
{
    // 대화창 생성 로직
    auto dialoguePanel = std::make_unique<DialoguePanel>(860.0f, 150.0f);
    m_dialoguePanel = dialoguePanel.get(); // 제어용 포인터 저장
    m_dialoguePanel->SetLocalPosition(20.0f, 530.0f);
    m_dialoguePanel->SetActive(false);
    m_uiList.push_back(std::move(dialoguePanel));

    // 공격 버튼 생성
    auto attackBtn = std::make_unique<UIButton>(L"공격", 120.0f, 40.0f);
    attackBtn->SetLocalPosition(30.0f, 470.0f);
    attackBtn->SetOnClick([this]() // this는 CombatState를 가리킴
        {
            if (m_combatManager.IsBattleEnd()) return;

            m_combatManager.PlayerAction(PLAYERACTION::ATTACK);

            if (m_dialoguePanel)
            {
                wchar_t buffer[256];
                if (m_combatManager.GetState() == BATTLESTATE::VICTORY)
                {
                    swprintf_s(buffer, 256, L"%s을/를 쓰러뜨렸다!", m_monster.name.c_str());
                }
                else if (m_combatManager.GetState() == BATTLESTATE::DEFEAT)
                {
                    swprintf_s(buffer, 256, L"플레이어가 쓰러졌다...");
                }
                m_dialoguePanel->PlayText(buffer);
            }
        });
    m_uiList.push_back(std::move(attackBtn));

    // 다음 상태로 넘어가기 위한 버튼 생성
    auto nextBtn = std::make_unique<UIButton>(L"다음", 120.0f, 40.0f);
    nextBtn->SetLocalPosition(100.0f, 470.0f);
    nextBtn->SetOnClick([this, pScene]() // pScene 캡처
        {
            if (m_combatManager.GetState() == BATTLESTATE::VICTORY)
            {
                pScene->IncreaseFloor();
                pScene->ChangeState(std::make_unique<RoomSelectState>());
            }
            else if (m_combatManager.GetState() == BATTLESTATE::DEFEAT)
            {
                wchar_t buffer[256];
                swprintf_s(buffer, 256, L"전투에서 패배했다. 다시 도전해보자");
                m_dialoguePanel->PlayText(buffer);
                SceneManager::Instance().ChangeScene(L"TitleScene");
            }
            else
            {
                wchar_t buffer[256];
                swprintf_s(buffer, 256, L"전투가 종료되어야지 진행할 수 있다.");
                m_dialoguePanel->PlayText(buffer);
            }
        });
    m_uiList.push_back(std::move(nextBtn));
}

void CombatState::Exit(PlayScene* pScene)
{
    m_uiList.clear();
    m_dialoguePanel = nullptr;
}