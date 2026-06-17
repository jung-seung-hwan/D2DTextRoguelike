#include "pch.h"
#include "TitleScene.h"

#include "TextRenderer.h"
#include "SceneManager.h"
#include "ResourceManager.h"

#include "UIObject.h"
#include "UIButton.h"
#include "UIImage.h"

void TitleScene::Initialize()
{
    ID2D1Bitmap* pBgBitmap = ResourceManager::Instance().GetBitmap(L"TitleBG");
    if (pBgBitmap != nullptr)
    {
        // 화면 해상도에 맞추어 이미지 객체 생성
        auto bgImage = std::make_unique<UIImage>(pBgBitmap, EngineConfig::SCREEN_WIDTH_F, EngineConfig::SCREEN_HEIGHT_F);
        bgImage->SetLocalPosition(0.0f, 0.0f);

        // 배경화면이므로 반드시 UI 요소들 중 가장 먼저 벡터에 삽입해야 뒤로 깔림
        m_uiList.push_back(std::move(bgImage));
    }

    auto startBtn = std::make_unique<UIButton>(L"Game Start", 300.0f, 50.0f);
    startBtn->SetFontKey(L"Title");
    startBtn->SetLocalPosition(EngineConfig::SCREEN_CENTER_X - (300.0f / 2.0f), EngineConfig::SCREEN_CENTER_Y + 200.f);
    startBtn->SetOnClick([]()
        {
            SceneManager::Instance().ChangeScene(L"PlayScene");
        });


    auto exitBtn = std::make_unique<UIButton>(L"Exit Game", 300.0f, 50.0f);
    exitBtn->SetFontKey(L"Title");
    exitBtn->SetLocalPosition(EngineConfig::SCREEN_CENTER_X - (300.0f / 2.0f), EngineConfig::SCREEN_CENTER_Y + 250.0f);
    exitBtn->SetOnClick([]()
        {
            PostQuitMessage(0);
        });


    m_uiList.push_back(std::move(startBtn));
    m_uiList.push_back(std::move(exitBtn));
}

void TitleScene::Update(float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void TitleScene::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    pTextRenderer->DrawText(
        L"TEXT ROGUELIKE",
        0.0f, 0.0f, 300.0f, 50.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );

    // 버튼 렌더링
    for (auto& ui : m_uiList)
    {
        ui->Render(pRenderer, pTextRenderer);
    }
}

void TitleScene::Release()
{
    m_uiList.clear();
}