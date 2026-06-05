#pragma once
#include "TextRenderer.h"

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    // 생명주기 함수
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) = 0;
    virtual void Release() = 0;
};