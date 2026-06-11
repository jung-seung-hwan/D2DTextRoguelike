#include "pch.h"
#include "UIRoomButton.h"
#include "InputManager.h"
#include "D2DRender.h"
#include "TextRenderer.h"

UIRoomButton::UIRoomButton(ID2D1Bitmap* pImage, const std::wstring& text, float width, float height)
    : m_pImage(pImage), m_text(text), m_isHovered(false), m_onClick(nullptr)
{
    // UIObject의 멤버 변수 m_size 초기화
    m_size = { width, height };
}

void UIRoomButton::Update(float deltaTime)
{
    // 부모의 Update를 호출하여 전역 좌표를 먼저 갱신
    UIObject::Update(deltaTime);

    if (!m_isActive) return;

    // 현재 마우스 좌표 획득
    POINT mousePos = InputManager::Instance().GetMousePos();

    // 전역 좌표와 크기를 활용한 충돌 영역 계산
    float left = m_globalPosition.x;
    float top = m_globalPosition.y;
    float right = left + m_size.width;
    float bottom = top + m_size.height;

    // 충돌 판정 및 상태 갱신
    if (mousePos.x >= left && mousePos.x <= right &&
        mousePos.y >= top && mousePos.y <= bottom)
    {
        m_isHovered = true;

        if (InputManager::Instance().IsMouseLeftClicked())
        {
            if (m_onClick != nullptr)
            {
                m_onClick();
            }
        }
    }
    else
    {
        m_isHovered = false;
    }
}

void UIRoomButton::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    // 기본 렌더링 영역 설정 (전역 좌표 기준)
    D2D1_RECT_F drawRect = {
        m_globalPosition.x,
        m_globalPosition.y,
        m_globalPosition.x + m_size.width,
        m_globalPosition.y + m_size.height
    };

    // 호버 연출: 영역을 사방으로 10픽셀씩 확장하여 이미지 스케일업 효과
    if (m_isHovered)
    {
        drawRect.left -= 10.0f;
        drawRect.right += 10.0f;
        drawRect.top -= 10.0f;
        drawRect.bottom += 10.0f;
    }

    // 문 이미지 렌더링
    if (m_pImage != nullptr)
    {
        pRenderer->DrawBitmap(m_pImage, drawRect);
    }

    // 상태에 따른 텍스트 색상 및 위치 조정
    D2D1::ColorF color = m_isHovered ? D2D1::ColorF(D2D1::ColorF::Yellow) : D2D1::ColorF(D2D1::ColorF::White);

    pTextRenderer->DrawText(
        m_text.c_str(),
        m_globalPosition.x, drawRect.bottom + 10.0f, // 이미지가 커지면 텍스트도 비율에 맞게 아래로 밀려남
        m_size.width, 40.0f,
        color,
        TextAlign::Center,
        VerticalAlign::Center
    );

    // 자식 객체 렌더링 보장
    UIObject::Render(pRenderer, pTextRenderer);
}