#include "pch.h"
#include "UIImage.h"

UIImage::UIImage(ID2D1Bitmap* pBitmap, float width, float height)
    : m_pBitmap(pBitmap)
{
    m_size = { width, height };
}

void UIImage::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible || m_pBitmap == nullptr) return;

    D2D1_RECT_F destRect = D2D1::RectF(
        m_globalPosition.x,
        m_globalPosition.y,
        m_globalPosition.x + m_size.width,
        m_globalPosition.y + m_size.height
    );

    // 투명도가 포함된 비트맵을 지정된 사각형 영역에 렌더링
    pRenderer->DrawBitmap(
        m_pBitmap,
        destRect
    );

    UIObject::Render(pRenderer, pTextRenderer);
}