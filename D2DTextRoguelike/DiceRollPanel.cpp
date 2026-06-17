#include "pch.h"
#include "DiceRollPanel.h"
#include "D2DRender.h"
#include "TextRenderer.h"
#include "ResourceManager.h"
#include "InputManager.h"

void DiceRollPanel::Open()
{
    m_result = DiceSystem::Result();

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
    m_result = DiceSystem::RollD20();

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

    float x = m_localPosition.x;
    float y = m_localPosition.y;

    m_diceRect = D2D1::RectF(
        x + 115.0f,
        y + 80.0f,
        x + 245.0f,
        y + 210.0f
    );

    if (!m_isRolling && !m_hasStartedRoll)
    {
        POINT mousePos = InputManager::Instance().GetMousePos();

        bool isInside =
            mousePos.x >= m_diceRect.left &&
            mousePos.x <= m_diceRect.right &&
            mousePos.y >= m_diceRect.top &&
            mousePos.y <= m_diceRect.bottom;

        if (isInside && InputManager::Instance().IsMouseLeftClicked())
        {
            StartRoll();
            return;
        }

        return;
    }

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

        if (!m_result.rolls.empty())
        {
            m_displayNumber = m_result.rolls.back().roll;
        }
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
        x,
        y + 25.0f,
        360.0f,
        40.0f,
        D2D1::ColorF(D2D1::ColorF::White),
        TextAlign::Center,
        VerticalAlign::Center
    );

    pTextRenderer->DrawText(
        L"주사위",
        x,
        y + 68.0f,
        360.0f,
        35.0f,
        D2D1::ColorF(0.75f, 0.65f, 0.5f),
        TextAlign::Center,
        VerticalAlign::Center
    );
    //pRenderer->DrawCircle(
    //    x + 180.0f,
    //    y + 145.0f,
    //    58.0f,
    //    D2D1::ColorF(0.85f, 0.75f, 0.55f)
    //);

    float offsetX = 0.0f;
    float scale = 1.0f;

    if (m_isRolling)
    {
        float t = m_currentTime / m_rollDuration;
        if (t > 1.0f) t = 1.0f;

        // 초반에는 크게 좌우로 흔들리고, 후반으로 갈수록 빠르게 감쇠
        float damping = 1.0f - t;
        damping = damping * damping;

        float swing = sinf(t * 3.141592f * 8.0f) * damping;

        offsetX = swing * 22.0f;

        // 살짝 눌렸다 펴지는 느낌만 추가
        scale = 1.0f + fabsf(swing) * 0.04f;

        // 마지막 15% 구간은 절도 있게 제자리로 고정
        if (t > 0.85f)
        {
            float snapT = (t - 0.85f) / 0.15f;
            offsetX *= (1.0f - snapT);
            scale = 1.0f + (scale - 1.0f) * (1.0f - snapT);
        }
    }

    float centerX = x + 180.0f + offsetX;
    float centerY = y + 155.0f;

    float baseSize = 150.0f;
    float size = baseSize * scale;
    float half = size * 0.5f;

    D2D1_RECT_F diceRect = D2D1::RectF(
        centerX - half,
        centerY - half,
        centerX + half,
        centerY + half
    );

    ID2D1Bitmap* diceBitmap = ResourceManager::Instance().GetBitmap(L"Dice");

    if (diceBitmap != nullptr)
    {
        pRenderer->DrawBitmap(diceBitmap, diceRect);
    }


    wchar_t numberBuffer[16];
    swprintf_s(numberBuffer, 16, L"%d", m_displayNumber);

    float diceCenterX = (diceRect.left + diceRect.right) * 0.5f;
    float diceCenterY = (diceRect.top + diceRect.bottom) * 0.5f;

    pTextRenderer->DrawText(
        numberBuffer,
        diceCenterX - 35.0f,
        diceCenterY - 30.0f,
        70.0f,
        60.0f,
        D2D1::ColorF(D2D1::ColorF::White),
        TextAlign::Center,
        VerticalAlign::Center
    );

}