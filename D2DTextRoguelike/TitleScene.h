#pragma once
#include "Scene.h"
#include "UIObject.h"

#include <vector>

class UIObject;

class TitleScene : public Scene
{
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;
    void Release() override;
private:
    std::vector<std::unique_ptr<UIObject>> m_uiList;
};