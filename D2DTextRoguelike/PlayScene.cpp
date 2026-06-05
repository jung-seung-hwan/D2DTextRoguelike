#include "pch.h"
#include "PlayScene.h"
#include "DataManager.h"
#include "InputManager.h"

void PlayScene::Initialize()
{
    // 몬스터 생성, 플레이어 초기화 등
    m_currentMonster = DataManager::Instance().GetMonsterData(L"MOB_SLIME");

    // 대화 패널 생성 및 등록
    auto dialoguePanel = std::make_unique<DialoguePanel>(860.0f, 150.0f);

    // 패널 제어를 위한 주소 확보
    m_dialoguePanel = dialoguePanel.get();
    m_dialoguePanel->SetLocalPosition(20.0f, 530.0f);
    m_dialoguePanel->SetActive(false);

    m_uiList.push_back(std::move(dialoguePanel));

    // 테스트 버튼 생성 및 등록
    auto testBtn = std::make_unique<UIButton>(L"대화 재생", 120.0f, 40.0f);
    testBtn->SetLocalPosition(20.0f, 470.0f);
    testBtn->SetOnClick([this]()
        {
            if (m_dialoguePanel != nullptr && !m_dialoguePanel->IsActive())
            {
                m_dialoguePanel->PlayText(L"앗! 야생의 슬라임이 튀어나왔다 앗! 야생의 슬라임이 튀어나왔다 앗! 야생의 슬라임이 튀어나왔다앗! 야생의 슬라임이 튀어나왔다!\n무엇을 할까?\n1. 공격  2. 도망");
            }
        });

    m_uiList.push_back(std::move(testBtn));
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
    // 화면 출력 테스트용 텍스트
    pTextRenderer->DrawText(
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

        pTextRenderer->DrawText(
            buffer,
            100.0f, 150.0f, 300.0f, 150.0f, // 텍스트가 그려질 사각형 영역
            D2D1::ColorF(D2D1::ColorF::White)
        );
    }

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