#pragma once
#include "SingletonBase.h"
#include "Scene.h"
#include "D2DRender.h"
#include "TextRenderer.h"
#include <unordered_map>
#include <string>
#include <wrl/client.h>

// 씬 전환 상태를 관리
enum class TransitionState
{
    None,
    FadeOut, // 화면이 어두워지는 중
    FadeIn   // 화면이 밝아지는 중
};

// 씬 매니저는 싱글톤으로 설정하여 하나의 매니저만 존재하도록 설정
class SceneManager : public SingletonBase<SceneManager>
{
    friend class SingletonBase<SceneManager>;

public:
    void Update(float deltaTime);
    void Render(myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer);
    void Release();

    void AddScene(const std::wstring& name, Scene* scene);
    void ChangeScene(const std::wstring& name);

private:
    SceneManager() = default;
    ~SceneManager() { Release(); }

    std::unordered_map<std::wstring, Scene*> m_scenes;

    Scene* m_currentScene = nullptr;
    Scene* m_nextScene = nullptr;

    // 페이드 연출용 데이터
    TransitionState m_transitionState = TransitionState::None;
    float m_fadeAlpha = 0.0f;
    float m_fadeSpeed = 1.0f;

};