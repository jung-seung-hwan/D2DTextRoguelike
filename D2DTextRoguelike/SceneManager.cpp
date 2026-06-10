#include "pch.h"
#include "SceneManager.h"

#include "Scene.h"
#include "D2DRender.h"

void SceneManager::Update(float deltaTime)
{
    // 페이드 상태 제어
    if (m_transitionState == TransitionState::FadeOut)
    {
        m_fadeAlpha += m_fadeSpeed * deltaTime;
        if (m_fadeAlpha >= 1.0f)
        {
            m_fadeAlpha = 1.0f;

            // 화면이 완전히 검게 변한 시점에 기존 씬을 날리고 새 씬을 초기화
            if (m_currentScene != nullptr)
            {
                m_currentScene->Release();
            }

            m_currentScene = m_nextScene;
            m_currentScene->Initialize();
            m_nextScene = nullptr;

            // 씬 교체가 끝났으므로 페이드 인으로 상태 전환
            m_transitionState = TransitionState::FadeIn;
        }
    }
    else if (m_transitionState == TransitionState::FadeIn)
    {
        m_fadeAlpha -= m_fadeSpeed * deltaTime;
        if (m_fadeAlpha <= 0.0f)
        {
            m_fadeAlpha = 0.0f;
            m_transitionState = TransitionState::None; // 전환 종료
        }
    }

    if (m_currentScene != nullptr)
    {
        m_currentScene->Update(deltaTime);
    }
}

void SceneManager::Render(myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer)
{
    if (m_currentScene != nullptr)
    {
        // 현재 활성화된 씬에게 그리기 도구를 그대로 전달
        // 매니저는 화면 갱신 요청만 전송하고 그림은 각각의 렌더링 파이프라인에서 처리
        m_currentScene->Render(m_pRenderer, pTextRenderer);
    }

    // 화면 전환 연출이 진행 중이라면 그 위에 반투명한 검은색 막 생성
    if (m_transitionState != TransitionState::None)
    {
        D2D1_SIZE_F rtSize = m_pRenderer->GetSize();

        D2D1_RECT_F fullScreenRect = D2D1::RectF(
            0.0f,
            0.0f,
            rtSize.width,
            rtSize.height
        );

        m_pRenderer->FillRectangle(
            fullScreenRect,
            D2D1::ColorF(0.0f, 0.0f, 0.0f, m_fadeAlpha)
        );
    }
}

void SceneManager::Release()
{

    if (m_currentScene != nullptr)
    {
        m_currentScene->Release();
        m_currentScene = nullptr;
    }

    for (auto& pair : m_scenes)
    {
        delete pair.second;
    }
    m_scenes.clear();
}

void SceneManager::AddScene(const std::wstring& name, Scene* scene)
{
    if (m_scenes.find(name) == m_scenes.end())
    {
        m_scenes[name] = scene;
    }
}

void SceneManager::ChangeScene(const std::wstring& name)
{
    auto it = m_scenes.find(name);
    if (it != m_scenes.end())
    {
        m_nextScene = it->second;
        m_transitionState = TransitionState::FadeOut;
    }
}