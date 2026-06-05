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

void UIPanel::SetBorder(bool hasBorder, float thickness, D2D1::ColorF color)
{
    m_hasBorder = hasBorder;
    m_borderThickness = thickness;
    m_borderColor = color;

    // 이미 브러시가 생성되어 있다면 색상만 교체
    if (m_pBorderBrush != nullptr)
    {
        m_pBorderBrush->SetColor(color);
    }
}

void UIPanel::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    /// 브러시가 아직 생성되지 않았을 때만 1회 생성
    if (m_pBrush == nullptr)
    {
        pContext->CreateSolidColorBrush(m_bgColor, &m_pBrush);
    }

    D2D1_RECT_F rect = D2D1::RectF(
        m_globalPosition.x,
        m_globalPosition.y,
        m_globalPosition.x + m_size.width,
        m_globalPosition.y + m_size.height
    );

    // 배경 색
    pContext->FillRectangle(rect, m_pBrush.Get());

    // 테두리 그리기
    if (m_hasBorder)
    {
        if (m_pBorderBrush == nullptr)
        {
            pContext->CreateSolidColorBrush(m_borderColor, &m_pBorderBrush);
        }
        // DrawRectangle의 3번째 매개변수로 선의 두께(Thickness)를 전달
        pContext->DrawRectangle(rect, m_pBorderBrush.Get(), m_borderThickness);
    }

    // 자식 UI 요소들(버튼, 텍스트 등) 렌더링
    UIObject::Render(pContext, pTextRenderer);
}

void UIPanel::SetBackgroundColor(D2D1::ColorF color)
{
    m_bgColor = color;

    // 브러시가 이미 존재한다면 새로 만들지 않고 색상만 갱신
    if (m_pBrush != nullptr)
    {
        m_pBrush->SetColor(color);
    }
}