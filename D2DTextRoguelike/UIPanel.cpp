#include "pch.h"
#include "UIPanel.h"

#include "D2DRender.h"

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

void UIPanel::SetBorder(bool hasBorder, float thickness, D2D1::ColorF color)
{
    m_hasBorder = hasBorder;
    m_borderThickness = thickness;
    m_borderColor = color;

}

void UIPanel::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    D2D1_RECT_F rect = D2D1::RectF(
        m_globalPosition.x,
        m_globalPosition.y,
        m_globalPosition.x + m_size.width,
        m_globalPosition.y + m_size.height
    );

    pRenderer->FillRectangle(rect, m_bgColor);

    if (m_hasBorder)
    {
        pRenderer->DrawRectangle(rect, m_borderColor, m_borderThickness);
    }

    UIObject::Render(pRenderer, pTextRenderer);
}

void UIPanel::SetBackgroundColor(D2D1::ColorF color)
{
    m_bgColor = color;

}