#pragma once
#include "UIPanel.h"
#include <string>

class DialoguePanel : public UIPanel
{
public:
    DialoguePanel(float width, float height);
    ~DialoguePanel() = default;

    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;

    // 텍스트 연출 시작
    void PlayText(const std::wstring& text);

    // 연출 스킵
    void SkipText();

    // 텍스트 출력이 모두 끝났는지 확인하는 상태자
    bool IsFinished() const { return m_isFinished; }

private:
    std::wstring m_fullText;        // 최종적으로 출력해야 할 전체 문장
    std::wstring m_currentText;     // 현재 프레임에 화면에 그려질 잘려진 문장

    float m_textSpeed;              // 글자당 출력 대기 시간
    float m_elapsedTime;            // 누적 시간
    size_t m_charIndex;             // 현재까지 잘라낸 글자의 개수

    bool m_isFinished;              // 애니메이션 완료 여부

    float m_inputCooldown = 0.0f;   // 입력 방지 유예 시간
};