#pragma once
#include "UIObject.h"

class DiceRollPanel : public UIObject
{
public:
    void StartRoll();
    bool IsRolling() const;
    bool IsFinished() const;

    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

private:
    bool m_isRolling = false;

    float m_rollDuration = 1.0f;
    float m_currentTime = 0.0f;

    float m_changeInterval = 0.05f;
    float m_changeTimer = 0.0f;

    int m_displayNumber = 1;
};