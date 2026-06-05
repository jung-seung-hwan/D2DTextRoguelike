#pragma once
#include "SingletonBase.h"
#include "Scene.h"
#include "TextRenderer.h"
#include <unordered_map>
#include <string>


// 씬 매니저는 싱글톤으로 설정하여 하나의 매니저만 존재하도록 설정
class SceneManager : public SingletonBase<SceneManager>
{
    friend class SingletonBase<SceneManager>;

public:
    void Update(float deltaTime);
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer);
    void Release();

    void AddScene(const std::wstring& name, Scene* scene);
    void ChangeScene(const std::wstring& name);

private:
    SceneManager() = default;
    ~SceneManager() { Release(); }

    std::unordered_map<std::wstring, Scene*> m_scenes;

    Scene* m_currentScene = nullptr;
    Scene* m_nextScene = nullptr;
};