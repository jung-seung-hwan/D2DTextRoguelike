#include "pch.h"
#include "CombatState.h"
#include "RoomSelectState.h" 
#include "PlayScene.h"

#include "D2DRender.h"
#include "TextRenderer.h"
#include "DataManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"

#include "UIObject.h"
#include "DialoguePanel.h"
#include "UIbutton.h"
#include "UIImage.h"
#include "StatusPanel.h"
#include "DiceRollPanel.h"

CombatState::CombatState(int floor, MonsterType type)
    : m_floor(floor), m_type(type)
{
}

void CombatState::Enter(PlayScene* pScene)
{
    m_player = pScene->GetPlayer();

    StartCombat(pScene);

    // 플레이어 상태창 생성 (HP 등)
    auto playerStatus = std::make_unique<StatusPanel>(
        pScene->GetPlayer(),
        360.0f, 12.0f
    );

    // 플레이어 상태창을 UI 목록에 등록
    playerStatus->SetLocalPosition(600.0f, 560.0f);
    m_uiList.push_back(std::move(playerStatus));

    // 몬스터 상태창
    auto monsterStatus = std::make_unique<StatusPanel>(
        &m_monster,
        300.0f,
        12.0f
    );
    monsterStatus->SetLocalPosition(70.0f, 60.0f);
    m_uiList.push_back(std::move(monsterStatus));

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

    if (m_diceRollPanel->IsFinished())
    {
        PLAYERACTION selectedAction = m_combatManager.GetPendingAction();
        m_combatManager.ApplyDiceResult(m_diceRollPanel->GetResult());

        //m_diceButton->SetActive(false);
        m_attackBtn->SetActive(false);
        m_defendBtn->SetActive(false);

        wchar_t buffer[256];
        wchar_t log1[128] = { 0, };

        if (selectedAction == PLAYERACTION::ATTACK)
        {
            int damageToMonster = m_combatManager.GetDamageToMonster();

            if (damageToMonster == -1)
            {
                swprintf_s(log1, 128, L"%s이(가) 공격을 회피했다!", m_monster.name.c_str());
            }
            else
            {
                swprintf_s(log1, 128, L"%s에게 %d의 피해를 입혔다.", m_monster.name.c_str(), damageToMonster);
            }
        }
        else if (selectedAction == PLAYERACTION::DEFEND)
        {
            swprintf_s(log1, 128, L"방어 태세를 취했다. 방어막이 생성되었다.");
        }

        // 전투 상태에 따른 분기 및 텍스트 조합
        if (m_combatManager.GetState() == BATTLESTATE::VICTORY)
        {
            // 버튼 비활성화
            m_attackBtn->SetActive(false);
            m_defendBtn->SetActive(false);
            //m_diceButton->SetActive(false);
            m_nextBtn->SetActive(false);
            // 승리 시: 공격 결과 + 몬스터 처치 메시지
            swprintf_s(buffer, 256, L"%s\n%s을(를) 쓰러뜨렸다!", log1, m_monster.name.c_str());

            // 드롭 아이템 획득 시도
            m_droppedItemData = DataManager::Instance().GetRandomItemData(m_floor);

            if (m_droppedItemData != nullptr)
            {
                // 현재 장착 중인 동일 스탯의 수치 확인
                int currentStat = pScene->GetPlayer()->GetInventory().GetStatBonus(m_droppedItemData->statType);

                // 능력치 종류를 UI 출력용 문자열로 변환
                std::wstring statName = L"";
                switch (m_droppedItemData->statType)
                {
                case ItemStatType::Attack:  statName = L"공격력"; break;
                case ItemStatType::Defense: statName = L"방어력"; break;
                case ItemStatType::Evasion: statName = L"회피율"; break;
                case ItemStatType::Hp: statName = L"체력"; break;
                }

                // 변환된 능력치 이름을 포함하여 텍스트 구성
                wchar_t rewardBuffer[512] = { 0, };
                swprintf_s(rewardBuffer, 512, L"%s\n\n전리품 발견: %s (%s +%d)\n현재 %s: +%d\n장비하시겠습니까?",
                    buffer,
                    m_droppedItemData->name.c_str(),
                    statName.c_str(),
                    m_droppedItemData->baseValue,
                    statName.c_str(),
                    currentStat);

                m_dialoguePanel->PlayText(rewardBuffer);

                // 장비 획득, 버리기 버튼 활성화
                m_equipBtn->SetActive(true);
                m_discardBtn->SetActive(true);
            }
            else
            {
                // 드롭 아이템이 없을 경우 바로 다음 버튼 활성화
                wchar_t emptyRewardBuffer[512] = { 0, };
                swprintf_s(emptyRewardBuffer, 512, L"%s\n아무것도 발견하지 못했다.", buffer);

                m_dialoguePanel->PlayText(emptyRewardBuffer);
                m_nextBtn->SetActive(true);
            }
        }
        else
        {
            swprintf_s(buffer, 256, L"%s\n%s의 턴입니다. 다음을 눌러 진행하세요.", log1, m_monster.name.c_str());
            m_dialoguePanel->PlayText(buffer);
        }

        if (m_combatManager.GetState() == BATTLESTATE::ENEMYTURN)
        {
            m_nextBtn->SetActive(true);
        }

        m_diceRollPanel->SetActive(false);
    }
}

void CombatState::Render(PlayScene* pScene, myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer)
{
    ID2D1Bitmap* pBgBitmap = nullptr;
    if (m_type == MonsterType::Boss || m_type == MonsterType::MidBoss)
    {
        // 보스용 배경 이미지 (ResourceManager에 미리 로드되어 있어야 함)
        pBgBitmap = ResourceManager::Instance().GetBitmap(L"BossBG");
    }
    else
    {
        // 일반 전투용 배경 이미지
        pBgBitmap = ResourceManager::Instance().GetBitmap(L"BattleBG");
    }

    if (pBgBitmap != nullptr)
    {
        m_pRenderer->DrawBitmap(pBgBitmap, D2D1::RectF(0.0f, 0.0f, EngineConfig::SCREEN_WIDTH_F, EngineConfig::SCREEN_HEIGHT_F));
    }

    ID2D1Bitmap* playerBitmap = ResourceManager::Instance().GetBitmap(L"Player");

    if (playerBitmap != nullptr)
    {
        // 밑에서 50.0f, 50.0f 띄우고 400x400 크기로 배치 < 해상도 수정됨
        m_pRenderer->DrawBitmap(
            playerBitmap,
            D2D1::RectF(70.0f, 360.0f, 560.0f, 850.0f) // (left, top, right, bottom)
        );
    }

    ID2D1Bitmap* slimeBitmap = ResourceManager::Instance().GetBitmap(L"Slime");

    if (slimeBitmap != nullptr)
    {
        m_pRenderer->DrawBitmap(
            slimeBitmap,
            D2D1::RectF(620.0f, 180.0f, 900.0f, 460.0f) // (left, top, right, bottom)
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

void CombatState::CreateUI(PlayScene* pScene)
{
    constexpr float dialogueX = 60.0f;
    constexpr float dialogueY = 800.0f;
    constexpr float buttonY = dialogueY - 48.0f;
    constexpr float buttonSpacing = 130.0f;
    constexpr float firstButtonX = dialogueX + 890.0f;

    constexpr float dicePanelX = 460.0f;
    constexpr float dicePanelY = 260.0f;
    constexpr float diceButtonX = dicePanelX + 120.0f;
    constexpr float diceButtonY = dicePanelY + 205.0f;

    // 대화창 생성 로직
    auto dialoguePanel = std::make_unique<DialoguePanel>(1160.0f, 170.0f);
    m_dialoguePanel = dialoguePanel.get(); // 제어용 포인터 저장
    m_dialoguePanel->SetLocalPosition(dialogueX, dialogueY);
    m_dialoguePanel->SetActive(false);
    m_uiList.push_back(std::move(dialoguePanel));

    // 공격 버튼 생성
    auto attackBtn = std::make_unique<UIButton>(L"공격", 120.0f, 40.0f);
    m_attackBtn = attackBtn.get();
    attackBtn->SetLocalPosition(firstButtonX, buttonY);
    attackBtn->SetOnClick([this]()
        {

            if (m_combatManager.GetState() != BATTLESTATE::PLAYERTURN)
                return;

            // 전투 연산 실행
            m_combatManager.SetAction(PLAYERACTION::ATTACK);

            if (m_dialoguePanel)
            {
                m_dialoguePanel->PlayText(L"공격을 선택했다.\n주사위를 굴려 행동을 실행하자.");


            }
            if (m_diceRollPanel)
            {
                m_diceRollPanel->Open();
            }

            //if (m_diceButton)
            //{
            //    m_diceButton->SetActive(true);
            //}
        });
    m_uiList.push_back(std::move(attackBtn));

    auto defendBtn = std::make_unique<UIButton>(L"방어", 120.0f, 40.0f);
    m_defendBtn = defendBtn.get();
    defendBtn->SetLocalPosition(firstButtonX + buttonSpacing, buttonY);
    defendBtn->SetOnClick([this]() {

        if (m_combatManager.GetState() != BATTLESTATE::PLAYERTURN)
            return;

        m_combatManager.SetAction(PLAYERACTION::DEFEND);

        if (m_dialoguePanel)
        {
            m_dialoguePanel->PlayText(L"방어를 선택했다.");
        }

        if (m_diceRollPanel)
        {
            m_diceRollPanel->Open();
        }

        //if (m_diceButton)
        //{
        //    m_diceButton->SetActive(true);
        //}
        });
    m_uiList.push_back(std::move(defendBtn));

    auto diceRollPanel = std::make_unique<DiceRollPanel>();
    m_diceRollPanel = diceRollPanel.get();
    m_diceRollPanel->SetLocalPosition(dicePanelX, dicePanelY);
    m_diceRollPanel->SetActive(false);
    m_uiList.push_back(std::move(diceRollPanel));

    //auto diceBtn = std::make_unique<UIButton>(L"굴리기", 120.0f, 40.0f);
    //m_diceButton = diceBtn.get();
    //m_diceButton->SetActive(false);    // 추가
    //diceBtn->SetLocalPosition(diceButtonX, diceButtonY);
    //diceBtn->SetOnClick([this]() {
    //    if (!m_diceRollPanel)
    //        return;

    //    m_diceButton->SetActive(false);
    //    m_diceRollPanel->StartRoll();

    //    });
    //m_uiList.push_back(std::move(diceBtn));

    // 보상 선택 버튼 - 장착
    auto equipBtn = std::make_unique<UIButton>(L"장착", 120.0f, 40.0f);
    m_equipBtn = equipBtn.get();
    m_equipBtn->SetLocalPosition(firstButtonX, buttonY);
    m_equipBtn->SetActive(false); // 초기 비활성
    m_equipBtn->SetOnClick([this, pScene]() {
        if (m_droppedItemData != nullptr)
        {
            auto newItem = std::make_unique<Item>(m_droppedItemData->name, m_droppedItemData->statType, m_droppedItemData->baseValue);
            pScene->GetPlayer()->GetInventory().EquipItem(std::move(newItem), pScene->GetPlayer());

            m_dialoguePanel->PlayText(L"새로운 장비를 장착했습니다.");
        }

        CompleteRewardSelection();
        });
    m_uiList.push_back(std::move(equipBtn));

    // 보상 선택 버튼 - 버리기
    auto discardBtn = std::make_unique<UIButton>(L"버리기", 120.0f, 40.0f);
    m_discardBtn = discardBtn.get();
    m_discardBtn->SetLocalPosition(firstButtonX + buttonSpacing, buttonY);
    m_discardBtn->SetActive(false); // 초기 비활성
    m_discardBtn->SetOnClick([this]() {
        m_dialoguePanel->PlayText(L"장비를 버리고 발걸음을 옮깁니다.");

        CompleteRewardSelection();
        });
    m_uiList.push_back(std::move(discardBtn));

    // 다음 상태로 넘어가기 위한 버튼 생성
    auto nextBtn = std::make_unique<UIButton>(L"다음", 120.0f, 40.0f);
    m_nextBtn = nextBtn.get();
    nextBtn->SetLocalPosition(firstButtonX + buttonSpacing, buttonY);
    m_nextBtn->SetActive(false);
    nextBtn->SetOnClick([this, pScene]() // pScene 캡처
        {
            if (m_combatManager.GetState() == BATTLESTATE::VICTORY)
            {
                m_attackBtn->SetActive(false);
                m_defendBtn->SetActive(false);
                if (m_floor >= 30)
                {
                    SceneManager::Instance().ChangeScene(L"EndingScene");
                }
                else
                {
                    pScene->IncreaseFloor();
                    pScene->ChangeState(std::make_unique<RoomSelectState>());
                }
            }
            else if (m_combatManager.GetState() == BATTLESTATE::DEFEAT)
            {
                m_attackBtn->SetActive(false);
                m_defendBtn->SetActive(false);
                //m_diceButton->SetActive(false);
                // 메시지 확인 여부에 따른 2단계 제어
                if (!m_isDefeatAcknowledged)
                {
                    wchar_t buffer[256];
                    swprintf_s(buffer, 256, L"전투에서 패배했다. 다시 도전해보자");
                    m_dialoguePanel->PlayText(buffer);

                    // 상태를 갱신하여 다음 클릭 시 씬 전환이 발생하도록 처리
                    m_isDefeatAcknowledged = true;
                }
                else
                {
                    SceneManager::Instance().ChangeScene(L"TitleScene");
                }
            }
            // 적 턴이 대기 중이면 버튼으로 적 턴 실행
            else if (m_combatManager.GetState() == BATTLESTATE::ENEMYTURN)
            {
                // 적 턴 수행
                m_combatManager.EnemyTurn();

                // 적 턴 결과 텍스트 구성
                wchar_t buffer[256] = { 0, };
                wchar_t log[256] = { 0, };

                int damageToPlayer = m_combatManager.GetDamageToPlayer();

                if (m_combatManager.GetState() == BATTLESTATE::DEFEAT)
                {
                    // 플레이어가 쓰러짐 — 기존 DEFEAT 흐름과 동일하게 메시지 출력 후 확인 대기
                    if (!m_isDefeatAcknowledged)
                    {
                        swprintf_s(buffer, 256, L"%s의 공격으로 플레이어가 쓰러졌습니다.\n계속하려면 다시 눌러주세요.", m_monster.name.c_str());
                        m_dialoguePanel->PlayText(buffer);
                        m_isDefeatAcknowledged = true;
                    }
                    else
                    {
                        SceneManager::Instance().ChangeScene(L"TitleScene");
                    }
                }
                else
                {
                    if (damageToPlayer == -1)
                    {
                        swprintf_s(log, 256, L"%s의 공격을 플레이어가 회피했다!", m_monster.name.c_str());
                    }
                    else if (damageToPlayer == 0)
                    {
                        swprintf_s(log, 256, L"방어막이 %s의 공격을 막아냈다.", m_monster.name.c_str());
                    }
                    else
                    {
                        swprintf_s(log, 256, L"%s의 공격! 플레이어가 %d의 피해를 입었다.", m_monster.name.c_str(), damageToPlayer);
                    }

                    // 턴 종료 안내
                    swprintf_s(buffer, 256, L"%s\n턴이 종료되었습니다. 다음은 당신의 턴입니다.", log);
                    m_dialoguePanel->PlayText(buffer);

                    m_nextBtn->SetActive(false);
                    m_attackBtn->SetActive(true);
                    m_defendBtn->SetActive(true);
                    //m_diceButton->SetActive(false);
                }
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
    m_diceRollPanel = nullptr;
}

void CombatState::CompleteRewardSelection()
{
    // 장착/버리기 버튼 숨김
    m_equipBtn->SetActive(false);
    m_discardBtn->SetActive(false);

    // 다음 층으로 이동 가능한 버튼 활성화
    m_nextBtn->SetActive(true);
}
