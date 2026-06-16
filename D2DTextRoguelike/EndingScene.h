#pragma once
#include "Scene.h"
#include "UIObject.h"

#include <vector>
#include <memory>

class UIObject;

class EndingScene : public Scene
{
public:
    EndingScene() = default;
    virtual ~EndingScene() = default;

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;
    virtual void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;
    virtual void Release() override;

private:
    std::vector<std::unique_ptr<UIObject>> m_uiList;
};