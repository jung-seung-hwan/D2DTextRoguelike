#pragma once
#include "UIObject.h"
#include <string>
#include <functional> // 콜백 함수 사용을 위해 포함

class UIButton : public UIObject
{
public:
    UIButton(const std::wstring& text, float width, float height);
    ~UIButton() = default;

    void Update(float deltaTime) override;
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;

    // 외부에서 클릭 시 실행할 동작을 등록하는 함수
    void SetOnClick(std::function<void()> onClick) { m_onClick = onClick; }

private:
    std::wstring m_text;
    bool m_isHovered; // 마우스가 버튼 위에 있는지 여부

    // 클릭 시 실행될 함수 (콜백)
    std::function<void()> m_onClick;
};