#include "pch.h"
#include "UIPanel.h"
#include <wrl/client.h> 

UIPanel::UIPanel(float width, float height)
    : m_bgColor(D2D1::ColorF(D2D1::ColorF::Black, 0.8f))
{
    m_size = { width, height };
}

void UIPanel::Update(float deltaTime)
{
    // 위치 계산 및 자식 갱신
    UIObject::Update(deltaTime);
}

void UIPanel::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    // 패널 배경 사각형 그리기
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    pContext->CreateSolidColorBrush(m_bgColor, &brush);

    D2D1_RECT_F rect = D2D1::RectF(
        m_globalPosition.x,
        m_globalPosition.y,
        m_globalPosition.x + m_size.width,
        m_globalPosition.y + m_size.height
    );

    pContext->FillRectangle(rect, brush.Get());

    // 자식 UI 요소들(버튼, 텍스트 등) 렌더링
    UIObject::Render(pContext, pTextRenderer);
}