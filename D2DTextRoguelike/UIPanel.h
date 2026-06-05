#pragma once
#include "UIObject.h"

class UIPanel : public UIObject
{
public:
    UIPanel(float width, float height);
    ~UIPanel() override = default;

    void Update(float deltaTime) override;
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;

    void SetBackgroundColor(D2D1::ColorF color) { m_bgColor = color; }

private:
    D2D1::ColorF m_bgColor;
};