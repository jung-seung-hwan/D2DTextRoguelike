#include "pch.h"
#include "UIObject.h"

UIObject::UIObject()
    : m_parent(nullptr), m_localPosition{ 0, 0 }, m_globalPosition{ 0, 0 },
    m_size{ 0, 0 }, m_isActive(true), m_isVisible(true)
{
}

void UIObject::Initialize()
{
}

void UIObject::Update(float deltaTime)
{
    if (!m_isActive) return;

    // 내 전역 좌표를 먼저 계산
    UpdateTransform();

    // 내 자식들에게 Update 전달
    for (auto& child : m_children)
    {
        child->Update(deltaTime);
    }
}

void UIObject::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    // 자식들에게 Render 전달
    for (auto& child : m_children)
    {
        child->Render(pRenderer, pTextRenderer);
    }
}

void UIObject::AddChild(std::unique_ptr<UIObject> child)
{
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

void UIObject::UpdateTransform()
{
    // 부모가 있다면, 부모의 전역 좌표에 내 지역 좌표를 더함
    if (m_parent != nullptr)
    {
        m_globalPosition.x = m_parent->m_globalPosition.x + m_localPosition.x;
        m_globalPosition.y = m_parent->m_globalPosition.y + m_localPosition.y;
    }
    else
    {
        // 최상단 UI라면 지역 좌표가 곧 전역 좌표
        m_globalPosition = m_localPosition;
    }
}