#include "pch.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "UIButton.h"
#include "UIPanel.h"

void TitleScene::Initialize()
{
    // 최상위 패널(창) 생성
    UIPanel* mainPanel = new UIPanel(400.0f, 600.0f);
    // 패널을 모바일 해상도(430x932)의 중앙 부근으로 배치
    mainPanel->SetLocalPosition(15.0f, 150.0f);
    mainPanel->SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));

    // 패널의 자식으로 버튼 생성 및 조립
    UIButton* startBtn = new UIButton(L"Game Start", 300.0f, 50.0f);
    // 좌표(50, 100)은 패널의 좌상단에서 우측 50, 아래 100만큼 떨어진 위치를 의미
    startBtn->SetLocalPosition(50.0f, 100.0f);
    startBtn->SetOnClick([]() { SceneManager::Instance().ChangeScene(L"PlayScene"); });
    mainPanel->AddChild(startBtn); // 배열이 아닌 패널에 넣음

    UIButton* exitBtn = new UIButton(L"Exit Game", 300.0f, 50.0f);
    exitBtn->SetLocalPosition(50.0f, 200.0f); // 패널 내부 좌표
    exitBtn->SetOnClick([]() { PostQuitMessage(0); });
    mainPanel->AddChild(exitBtn); // 배열이 아닌 패널에 넣음

    // 씬의 장부(m_uiList)에는 완성된 패널 단 1개만 등록
    m_uiList.push_back(mainPanel);
}

void TitleScene::Update(float deltaTime)
{
    for (auto ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void TitleScene::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    pTextRenderer->DrawText(
        L"TEXT ROGUELIKE",
        0.0f, 0.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );

    // 버튼 렌더링
    for (auto ui : m_uiList)
    {
        ui->Render(pContext, pTextRenderer);
    }
}

void TitleScene::Release()
{
    for (auto ui : m_uiList)
    {
        ui->Release();
        delete ui;
    }
    m_uiList.clear();
}