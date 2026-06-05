#include "pch.h"
#include "DialoguePanel.h"
#include "InputManager.h"

DialoguePanel::DialoguePanel(float width, float height)
    : UIPanel(width, height), m_textSpeed(0.03f), m_elapsedTime(0.0f), m_charIndex(0), m_isFinished(true)
{
    // 검은색 배경 세팅
    SetBackgroundColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

    // 흰색 테두리 추가
    SetBorder(true, 4.0f, D2D1::ColorF(D2D1::ColorF::White));
}

void DialoguePanel::PlayText(const std::wstring& text)
{
    m_fullText = text;
    m_currentText = L"";
    m_charIndex = 0;
    m_elapsedTime = 0.0f;
    m_isFinished = false;
    m_inputCooldown = 0.2f;

    // 텍스트가 시작될 때 패널을 화면에 표시
    SetActive(true);
    UpdateTransform();
}

void DialoguePanel::SkipText()
{
    // 출력이 진행 중일 때 호출되면, 남은 과정을 무시하고 즉시 전체를 띄움
    if (!m_isFinished)
    {
        m_currentText = m_fullText;
        m_charIndex = m_fullText.length();
        m_isFinished = true;
    }
}

void DialoguePanel::Update(float deltaTime)
{
    if (!m_isActive) return;

    // 패널 자체의 좌표 갱신
    UIPanel::Update(deltaTime);

    if (m_inputCooldown > 0.0f)
    {
        m_inputCooldown -= deltaTime;
    }

    // 패널 내부에서 자체적으로 입력을 처리하여 상태 제어
    if (m_inputCooldown <= 0.0f)
    {
        if (InputManager::Instance().IsKeyDown(VK_SPACE) || InputManager::Instance().IsMouseLeftDown())
        {
            if (!m_isFinished)
            {
                SkipText();
                m_inputCooldown = 0.2f; // 스킵 직후 발생하는 잔여 입력 무시
            }
        }
    }

    // 타자기 연출 로직
    if (!m_isFinished)
    {
        m_elapsedTime += deltaTime;

        // 누적 시간이 설정된 글자당 지연 시간을 초과했을 경우
        while (m_elapsedTime >= m_textSpeed && !m_isFinished)
        {
            m_elapsedTime -= m_textSpeed;
            m_charIndex++;

            if (m_charIndex >= m_fullText.length())
            {
                m_charIndex = m_fullText.length();
                m_isFinished = true;
            }
        }

        m_currentText = m_fullText.substr(0, m_charIndex);
    }
}

void DialoguePanel::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    // UIPanel로 배경 사각형 렌더링
    UIPanel::Render(pContext, pTextRenderer);
    if (m_currentText.empty()) return;

    // 현재까지 진행된 텍스트 렌더링
    pTextRenderer->DrawText(
        m_currentText,
        m_globalPosition.x + 20.0f,
        m_globalPosition.y + 20.0f,
        m_size.width - 40.0f,
        m_size.height - 40.0f,
        D2D1::ColorF(D2D1::ColorF::White)
    );
}