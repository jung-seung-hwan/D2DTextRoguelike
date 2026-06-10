#pragma once
#include <vector>
#include <d2d1_3.h>
#include <memory>

class TextRenderer;

namespace myspace
{
    class D2DRenderer;
}

// 모든 ui의 부모 클래스
class UIObject
{
public:
    UIObject();
    ~UIObject() = default;

    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer);

    // 계층 구조 관리 -> 부모 UI가 자식 UI의 소유권을 가지며,
    // UIObject 소멸 시 자식 객체도 자동으로 해제하기 위해 unique_ptr 사용
    void AddChild(std::unique_ptr<UIObject> child);

    // 위치 계산 로직 (지역 좌표 -> 전역 좌표 변환)
    void UpdateTransform();

    // 상태 제어 접근자
    // 부모 기준 위치와 실제 화면 기준 위치를 저장
    void SetLocalPosition(float x, float y) { m_localPosition = { x, y }; UpdateTransform(); }
    D2D1_POINT_2F GetGlobalPosition() const { return m_globalPosition; }

    void SetActive(bool active) { m_isActive = active; }
    bool IsActive() const { return m_isActive; }

protected:
    UIObject* m_parent;
    std::vector<std::unique_ptr<UIObject>> m_children;

    // 공간 데이터
    D2D1_POINT_2F m_localPosition;  // 부모를 기준으로 한 상대 좌표
    D2D1_POINT_2F m_globalPosition; // 실제 화면에 그려질 절대 좌표
    D2D1_SIZE_F m_size;

    // 상태 데이터
    bool m_isActive;  // Update와 Render를 모두 수행할지 여부
    bool m_isVisible; // Update는 하되 화면에 그리기만 생략할지 여부
};