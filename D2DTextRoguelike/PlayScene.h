#pragma once
#include "Scene.h"
#include "TextRenderer.h"
#include <vector>
#include "DataManager.h"
#include "UIObject.h"

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
    std::vector<std::unique_ptr<UIObject>> m_uiList;
    const MonsterData* m_currentMonster = nullptr;
};