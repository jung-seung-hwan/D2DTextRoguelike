#include "pch.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "UIButton.h"
#include "UIPanel.h"

void TitleScene::Initialize()
{
    // 패널 생성
    auto mainPanel = std::make_unique<UIPanel>(400.0f, 600.0f);

    mainPanel->SetLocalPosition(15.0f, 150.0f);
    mainPanel->SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));

    auto startBtn = std::make_unique<UIButton>(L"Game Start", 300.0f, 50.0f);
    startBtn->SetLocalPosition(50.0f, 100.0f);
    startBtn->SetOnClick([]()
        {
            SceneManager::Instance().ChangeScene(L"PlayScene");
        });

    mainPanel->AddChild(std::move(startBtn));

    auto exitBtn = std::make_unique<UIButton>(L"Exit Game", 300.0f, 50.0f);
    exitBtn->SetLocalPosition(50.0f, 200.0f);
    exitBtn->SetOnClick([]()
        {
            PostQuitMessage(0);
        });

    mainPanel->AddChild(std::move(exitBtn));

    // 씬에는 완성된 패널 1개만 등록
    m_uiList.push_back(std::move(mainPanel));
}

void TitleScene::Update(float deltaTime)
{
    for (auto& ui : m_uiList)
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
    for (auto& ui : m_uiList)
    {
        ui->Render(pContext, pTextRenderer);
    }
}

void TitleScene::Release()
{
    m_uiList.clear();
}