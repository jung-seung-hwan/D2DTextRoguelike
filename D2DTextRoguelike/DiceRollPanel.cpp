#include "pch.h"
#include "DiceRollPanel.h"
#include "D2DRender.h"
#include "TextRenderer.h"

void DiceRollPanel::Open()
{
    m_isRolling = false;
    m_hasStartedRoll = false;
    m_currentTime = 0.0f;
    m_changeTimer = 0.0f;
    m_finishHoldTimer = 0.0f;
    m_displayNumber = 1;

    SetActive(true);
}

void DiceRollPanel::StartRoll()
{
    m_isRolling = true;
    m_hasStartedRoll = true;
    m_currentTime = 0.0f;
    m_changeTimer = 0.0f;
    m_finishHoldTimer = 0.0f;
    m_displayNumber = 1;

    SetActive(true);
}

bool DiceRollPanel::IsRolling() const
{
    return m_isRolling;
}

bool DiceRollPanel::IsFinished() const
{
    return IsActive() &&
        m_hasStartedRoll &&
        !m_isRolling &&
        m_finishHoldTimer >= m_finishHoldTime;
}



void DiceRollPanel::Update(float deltaTime)
{
    if (!IsActive())
        return;

    if (!m_isRolling)
    {
        if (m_hasStartedRoll)
        {
            m_finishHoldTimer += deltaTime;
        }
        return;
    }

    m_currentTime += deltaTime;
    m_changeTimer += deltaTime;

    if (m_changeTimer >= m_changeInterval)
    {
        m_changeTimer = 0.0f;
        m_displayNumber = rand() % 20 + 1;
    }

    if (m_currentTime >= m_rollDuration)
    {
        m_isRolling = false;
        m_finishHoldTimer = 0.0f;

    }
}

void DiceRollPanel::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (!IsActive())
        return;

    float x = m_localPosition.x;
    float y = m_localPosition.y;

    D2D1_RECT_F bgRect = D2D1::RectF(x, y, x + 360.0f, y + 260.0f);

    pRenderer->FillRectangle(
        bgRect,
        D2D1::ColorF(0.02f, 0.02f, 0.03f, 0.85f)
    );

    pRenderer->DrawRectangle(
        bgRect,
        D2D1::ColorF(0.8f, 0.65f, 0.35f),
        2.0f
    );

    pTextRenderer->DrawText(
        L"행동 판정",
        x + 115.0f, y + 25.0f,
        180.0f, 40.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );

    pTextRenderer->DrawText(
        L"주사위",
        x + 145.0f, y + 70.0f,
        120.0f, 35.0f,
        D2D1::ColorF(0.75f, 0.65f, 0.5f)
    );

    pRenderer->DrawCircle(
        x + 180.0f,
        y + 145.0f,
        58.0f,
        D2D1::ColorF(0.85f, 0.75f, 0.55f)
    );

    wchar_t numberBuffer[16];
    swprintf_s(numberBuffer, 16, L"%d", m_displayNumber);

    pTextRenderer->DrawText(
        numberBuffer,
        x + 155.0f, y + 115.0f,
        80.0f, 70.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );

    if (m_isRolling)
    {
        pTextRenderer->DrawText(
            L"굴리는 중...",
            x + 120.0f, y + 210.0f,
            160.0f, 35.0f,
            D2D1::ColorF(0.75f, 0.65f, 0.5f)
        );
    }
}