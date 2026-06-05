#pragma once
#include "SimpleMathHelper.h"
//#include <d2d1helper.h>
#include <vector>
#include <algorithm>

using namespace MYHelper;
namespace D2DTM
{
    enum class PivotPreset
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    class Transform
    {
    public:
        using Vec2 = MYHelper::Vector2F;
        using Mat3x2 = D2D1::Matrix3x2F;

        Transform()
            : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
            m_dirty(false), m_parent(nullptr)
        {
            m_matrixLocal = D2D1::Matrix3x2F::Identity();
            m_matrixWorld = D2D1::Matrix3x2F::Identity();
        }

        ~Transform()
        {
            for (auto* child : m_children)
                child->m_parent = nullptr;
        }

        // ** 부모-자식 계층 구조 **
        void SetParent(Transform* newParent)
        {
            if (m_parent)
                m_parent->RemoveChild(this);

            m_parent = newParent;

            if (m_parent)
                m_parent->AddChild(this);

            SetDirty();
        }

        void DetachFromParent()
        {
            SetParent(nullptr);
        }

        void AddChild(Transform* child)
        {
            m_children.push_back(child);
        }

        void RemoveChild(Transform* child)
        {
            m_children.erase(
                std::remove(m_children.begin(), m_children.end(), child),
                m_children.end()
            );
        }

        // ** 트랜스폼 설정 **
        void SetPosition(const Vec2& pos) { m_position = pos; SetDirty(); }
        void SetRotation(float degree) { m_rotation = degree; SetDirty(); }
        void SetScale(const Vec2& scale) { m_scale = scale; SetDirty(); }

        const Vec2& GetPosition() const { return m_position; }
        float GetRotation() const { return m_rotation; }
        const Vec2& GetScale() const { return m_scale; }

        void Translate(const Vec2& delta)
        {
            m_position.x += delta.x;
            m_position.y += delta.y;
            SetDirty();
        }

        void Translate(const float x, const float y)
        {
            m_position.x += x;
            m_position.y += y;
            SetDirty();
        }

        void Rotate(float degree)
        {
            m_rotation += degree;
            SetDirty();
        }

        // ** 방향 벡터 **
        Vec2 GetForward() const
        {
            float radian = DegreeToRadian(m_rotation);
            return { std::cosf(radian), std::sinf(radian) };
        }

        // ** 변환 행렬 **
        const Mat3x2& GetLocalMatrix()
        {
            if (m_dirty) UpdateMatrices();

            return m_matrixLocal;
        }

        const Mat3x2& GetWorldMatrix()
        {
            if (m_dirty) UpdateMatrices();

            return m_matrixWorld;
        }

        Mat3x2 GetInverseWorldMatrix()
        {
            Mat3x2 inv = GetWorldMatrix();
            inv.Invert();
            return inv;
        }

        // ** 회전과 스케일을 위한 피봇 설정 **
        void SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size);

        D2D1_POINT_2F GetPivotPoint() const
        {
            return m_pivot;
        }

    private:
        void SetDirty()
        {
            m_dirty = true;
            for (auto* child : m_children)
            {
                child->SetDirty();
            }
        }

        void UpdateMatrices();

    private:

        bool m_dirty;

        Vec2     m_position = { 0.f, 0.f };     // translation position
        float    m_rotation = 0.f;          // in degrees
        Vec2     m_scale = { 1.f, 1.f };


        Transform* m_parent;
        std::vector<Transform*> m_children;

        Mat3x2 m_matrixLocal;
        Mat3x2 m_matrixWorld;

        D2D1_POINT_2F m_pivot{ 0.0f, 0.0f };

    public:
#ifdef _DEBUG
        const std::wstring& WorldMatixString() const;
        const std::wstring& LocalMatrixString() const;

#endif

    };


    inline D2D1::Matrix3x2F RemovePivot(const D2D1::Matrix3x2F& M_local, const D2D1_POINT_2F& pivot)
    {
        // 1) pivot 보정 전(원점으로) : T(-pivot)
        auto P0 = D2D1::Matrix3x2F::Translation(-pivot.x, -pivot.y);
        // 2) pivot 복원 후         : T(+pivot)
        auto P1 = D2D1::Matrix3x2F::Translation(pivot.x, pivot.y);
        // 3) 양쪽에 곱해서 피벗만 제거
        //    P1 * (P0 * M_local * P1) * P0 == M_local 이지만
        //    pivot 보정만 빼려면: M_noPivot = P1 * M_local * P0
        return P1 * M_local * P0;
    }

    inline void DecomposeMatrix3x2(
        const D2D1::Matrix3x2F& M,
        Vector2F& outTranslation,
        float& outRotation,
        Vector2F& outScale
    )
    {
        // 1) Translation
        outTranslation.x = M._31;
        outTranslation.y = M._32;

        // 2) Scale: 각 축 벡터의 길이

        outScale.x = sqrtf(M._11 * M._11 + M._12 * M._12);
        outScale.y = sqrtf(M._21 * M._21 + M._22 * M._22);

        // 3) Rotation (rad → deg):
        // M = [ m11  m12  ]
        //     [ m21  m22  ]
        // rotation = atan2(m12/sx, m11/sx) == atan2(m12, m11)
        // θ_rad = atan2(m12, m11)
        // θ_deg = θ_rad * (180/π)
        const float radians = atan2(M._12, M._11);

        outRotation = RadianToDegree(radians);
    }
}

