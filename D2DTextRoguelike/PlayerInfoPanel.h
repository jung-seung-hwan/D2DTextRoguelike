#pragma once
#include "UIObject.h"
#include "Player.h"

class D2dRenderer;
class TextRenderer;

class PlayerInfoPanel : public UIObject
{
public:
    PlayerInfoPanel(Player* player, float width, float height);
    virtual ~PlayerInfoPanel() = default;

    virtual void Update(float deltaTime) override;
    virtual void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

private:
    Player* m_player = nullptr;
};