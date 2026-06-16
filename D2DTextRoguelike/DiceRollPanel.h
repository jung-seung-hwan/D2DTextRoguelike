#pragma once
#include "UIObject.h"
#include "DiceSystem.h"

class DiceRollPanel : public UIObject
{
public:
    void Open();
    void StartRoll();

    bool IsRolling() const;
    bool IsFinished() const;

    const DiceSystem::Result& GetResult() const
    {
        return m_result;
    }

    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

private:
    bool m_isRolling = false;
    bool m_hasStartedRoll = false;

    DiceSystem::Result m_result;

    float m_rollDuration = 1.0f;
    float m_currentTime = 0.0f;

    float m_changeInterval = 0.05f;
    float m_changeTimer = 0.0f;

    float m_finishHoldTime = 0.5f;
    float m_finishHoldTimer = 0.0f;

    int m_displayNumber = 1;
};