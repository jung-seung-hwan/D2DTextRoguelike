#include "pch.h"
#include "UIButton.h"
#include "InputManager.h"

UIButton::UIButton(const std::wstring& text, float width, float height)
    : m_text(text), m_isHovered(false), m_onClick(nullptr)
{
    // UIObject의 멤버 변수 m_size 초기화
    m_size = { width, height };
}

void UIButton::Update(float deltaTime)
{
    // 부모의 Update를 호출하여 전역 좌표를 먼저 갱신
    UIObject::Update(deltaTime);

    if (!m_isActive) return;

    // 현재 마우스 좌표 획득
    POINT mousePos = InputManager::Instance().GetMousePos();

    // AABB (Axis-Aligned Bounding Box) 충돌 판정
    float left = m_globalPosition.x;
    float top = m_globalPosition.y;
    float right = left + m_size.width;
    float bottom = top + m_size.height;

    // 마우스가 버튼 사각형 영역 내부에 있는지 검사
    if (mousePos.x >= left && mousePos.x <= right &&
        mousePos.y >= top && mousePos.y <= bottom)
    {
        m_isHovered = true;

        // 마우스 왼쪽 버튼이 이번 프레임에 '막 클릭' 되었는지 검사
        if (InputManager::Instance().IsMouseLeftClicked())
        {
            // 등록된 동작이 있다면 실행
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

void UIButton::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    // 마우스가 올려져 있으면 노란색, 아니면 흰색으로 텍스트 색상 변경
    D2D1::ColorF color = m_isHovered ? D2D1::ColorF(D2D1::ColorF::Yellow) : D2D1::ColorF(D2D1::ColorF::White);

    // 버튼 영역을 기준으로 텍스트 출력
    pTextRenderer->DrawTextW(
        m_text.c_str(),
        m_globalPosition.x, m_globalPosition.y, m_size.width, m_size.height,
        color
    );

    // 자식 UI가 있다면 마저 그리도록 부모 Render 호출
    UIObject::Render(pContext, pTextRenderer);
}