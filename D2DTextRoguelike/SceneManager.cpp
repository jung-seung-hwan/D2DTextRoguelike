#include "pch.h"
#include "SceneManager.h"

void SceneManager::Update(float deltaTime)
{
    // 씬 전환 예약이 있다면, 이번 프레임 시작 시점에 메모리 교체
    if (m_nextScene != nullptr)
    {
        if (m_currentScene != nullptr)
        {
            m_currentScene->Release();
        }

        m_currentScene = m_nextScene;
        m_currentScene->Initialize();

        m_nextScene = nullptr;
    }

    if (m_currentScene != nullptr)
    {
        m_currentScene->Update(deltaTime);
    }
}

void SceneManager::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    if (m_currentScene != nullptr)
    {
        // 현재 활성화된 씬에게 그리기 도구를 그대로 전달
        m_currentScene->Render(pContext, pTextRenderer);
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
    }
}