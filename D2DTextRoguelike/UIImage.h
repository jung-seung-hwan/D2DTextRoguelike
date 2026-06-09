#pragma once
#include "UIObject.h"
#include <d2d1_3.h>

class UIImage : public UIObject
{
public:
    UIImage(ID2D1Bitmap* pBitmap, float width, float height);
    ~UIImage() = default;

    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

private:
    ID2D1Bitmap* m_pBitmap = nullptr; // ∑ª¥ı∏µ«“ ∫Ò∆Æ∏ ¿« ¡÷º“
};