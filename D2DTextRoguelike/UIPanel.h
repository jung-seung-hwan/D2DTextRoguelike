#pragma once
#include "UIObject.h"

class UIPanel : public UIObject
{
public:
    UIPanel(float width, float height);
    ~UIPanel() = default;

    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

    void SetBackgroundColor(D2D1::ColorF color);

    // 테두리 설정
    void SetBorder(bool hasBorder, float thickness = 2.0f, D2D1::ColorF color = D2D1::ColorF(D2D1::ColorF::White));

private:
    D2D1::ColorF m_bgColor;

    // 테두리 관련 데이터
    bool m_hasBorder = false;
    float m_borderThickness = 2.0f;
    D2D1::ColorF m_borderColor = D2D1::ColorF(D2D1::ColorF::White);
};