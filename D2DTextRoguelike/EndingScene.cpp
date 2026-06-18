#include "pch.h"
#include "EndingScene.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "D2DRender.h"
#include "TextRenderer.h"
#include "UIButton.h"

void EndingScene::Initialize()
{
    // 타이틀로 돌아가기 버튼
    auto titleBtn = std::make_unique<UIButton>(L"타이틀로 돌아가기", 200.0f, 60.0f);
    titleBtn->SetFontKey(L"Default");

    // 화면 하단 중앙에 배치
    float btnX = (EngineConfig::SCREEN_WIDTH_F / 2.0f) - 100.0f;
    float btnY = EngineConfig::SCREEN_HEIGHT_F - 150.0f;
    titleBtn->SetLocalPosition(btnX, btnY);

    // 클릭 이벤트: 타이틀 씬으로 전환
    titleBtn->SetOnClick([]() {
        SceneManager::Instance().ChangeScene(L"TitleScene");
        });

    m_uiList.push_back(std::move(titleBtn));
}

void EndingScene::Update(float deltaTime)
{
    for (auto& ui : m_uiList)
    {
        ui->Update(deltaTime);
    }
}

void EndingScene::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    // 엔딩 배경 이미지 렌더링
    ID2D1Bitmap* pBgBitmap = ResourceManager::Instance().GetBitmap(L"EndingBG");
    if (pBgBitmap != nullptr)
    {
        pRenderer->DrawBitmap(pBgBitmap, D2D1::RectF(0.0f, 0.0f, EngineConfig::SCREEN_WIDTH_F, EngineConfig::SCREEN_HEIGHT_F));
    }

    for (auto& ui : m_uiList)
    {
        ui->Render(pRenderer, pTextRenderer);
    }
}

void EndingScene::Release()
{
    // 씬 종료 시 컨테이너 비우기를 통한 UI 메모리 일괄 해제
    m_uiList.clear();
}