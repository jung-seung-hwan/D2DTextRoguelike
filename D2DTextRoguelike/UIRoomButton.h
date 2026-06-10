#pragma once

#include "UIObject.h"
#include <string>
#include <functional>

namespace myspace { class D2DRenderer; }
class TextRenderer;
struct ID2D1Bitmap;

class UIRoomButton : public UIObject
{
public:
    UIRoomButton(ID2D1Bitmap* pImage, const std::wstring& text, float width, float height);
    virtual ~UIRoomButton() = default;

    virtual void Update(float deltaTime) override;
    virtual void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

    void SetOnClick(std::function<void()> onClick) { m_onClick = onClick; }

private:
    ID2D1Bitmap* m_pImage = nullptr;
    std::wstring m_text;
    bool m_isHovered = false;

    std::function<void()> m_onClick;
};