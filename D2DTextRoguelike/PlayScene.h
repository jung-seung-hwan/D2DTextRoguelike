#pragma once
#include "Scene.h"
#include "TextRenderer.h"
#include <vector>
#include "DataManager.h"

class UIObject;

class PlayScene : public Scene
{
public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;
    void Release() override;
private:
    std::vector<UIObject*> m_uiList;
    const MonsterData* m_currentMonster = nullptr;
};