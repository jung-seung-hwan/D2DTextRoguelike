#pragma once
#include "Scene.h"
#include "TextRenderer.h"
#include "UIObject.h"
#include <vector>


class TitleScene : public Scene
{
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;
    void Release() override;
private:
    std::vector<std::unique_ptr<UIObject>> m_uiList;
};