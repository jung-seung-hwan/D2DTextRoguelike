#pragma once
#include "UIObject.h"

class UIPanel : public UIObject
{
public:
    UIPanel(float width, float height);
    ~UIPanel() = default;

    void Update(float deltaTime) override;
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;

    void SetBackgroundColor(D2D1::ColorF color);

private:
    D2D1::ColorF m_bgColor;

    // 브러시를 캐싱할 스마트 포인터 멤버 변수
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBrush;
};