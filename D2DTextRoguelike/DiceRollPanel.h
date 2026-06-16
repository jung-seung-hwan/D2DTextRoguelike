#pragma once
#include "UIObject.h"

class DiceRollPanel : public UIObject
{
public:
    void Open();
    void StartRoll();
    bool IsRolling() const;
    bool IsFinished() const;

    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

private:
    bool m_isRolling = false;
    bool m_hasStartedRoll = false;

    float m_finishHoldTime = 0.5f;
    float m_finishHoldTimer = 0.0f;

    float m_rollDuration = 1.0f;
    float m_currentTime = 0.0f;

    float m_changeInterval = 0.05f;
    float m_changeTimer = 0.0f;

    int m_displayNumber = 1;
};