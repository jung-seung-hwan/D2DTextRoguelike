#include "pch.h"
#include "UIObject.h"

UIObject::UIObject()
    : m_parent(nullptr), m_localPosition{ 0, 0 }, m_globalPosition{ 0, 0 },
    m_size{ 0, 0 }, m_isActive(true), m_isVisible(true)
{
}

UIObject::~UIObject()
{
    Release();
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
    for (auto child : m_children)
    {
        child->Update(deltaTime);
    }
}

void UIObject::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible) return;

    // 자식들에게 Render 전달
    for (auto child : m_children)
    {
        child->Render(pContext, pTextRenderer);
    }
}

void UIObject::Release()
{
    // 자식들의 메모리를 일괄 해제
    for (auto child : m_children)
    {
        child->Release();
        delete child;
    }
    m_children.clear();
}

void UIObject::AddChild(UIObject* child)
{
    if (child != nullptr)
    {
        child->m_parent = this;
        m_children.push_back(child);
    }
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