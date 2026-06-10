#pragma once
class TextRenderer;

namespace myspace
{
    class D2DRenderer;
}

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    // 전체 씬에서 구현해야할 생명주기 함수
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) = 0;
    virtual void Release() = 0;
};