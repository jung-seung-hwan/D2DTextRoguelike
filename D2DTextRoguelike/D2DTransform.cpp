#include "pch.h"
#include "D2DTransform.h"

namespace D2DTM
{
	std::wstring MakeMatrixToString(const D2D1_MATRIX_3X2_F& matrix)
	{
		return std::to_wstring(matrix._11) + L",\t" + std::to_wstring(matrix._12) + L", " +
			std::to_wstring(matrix._21) + L",\t" + std::to_wstring(matrix._22) + L", " +
			std::to_wstring(matrix._31) + L",\t" + std::to_wstring(matrix._32);
	}

	void Transform::SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size)
	{
		switch (preset)
		{
		case PivotPreset::TopLeft:
			m_pivot = { 0.0f, 0.0f };
			break;
		case PivotPreset::TopRight:
			m_pivot = { size.width, 0.0f };
			break;
		case PivotPreset::BottomLeft:
			m_pivot = { 0.0f, -size.height };
			break;
		case PivotPreset::BottomRight:
			m_pivot = { size.width, -size.height };
			break;
		case PivotPreset::Center:
			m_pivot = { size.width * 0.5f, -(size.height * 0.5f) };

			break;
		}
	}


	void Transform::UpdateMatrices()
	{
		const auto P = D2D1::Matrix3x2F::Translation(-m_pivot.x, -m_pivot.y);

		const auto S = D2D1::Matrix3x2F::Scale(m_scale.x, m_scale.y);

		const auto R = D2D1::Matrix3x2F::Rotation(m_rotation);

		const auto T1 = D2D1::Matrix3x2F::Translation(m_pivot.x, m_pivot.y);

		const auto T2 = D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);

		m_matrixLocal = P * S * R * T1 * T2;
		//m_matrixLocal = S * R * T2;

		if (m_parent)
			m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
		else
			m_matrixWorld = m_matrixLocal;

		m_dirty = false;
	}
#ifdef _DEBUG
	const std::wstring& Transform::WorldMatixString() const
	{
		static std::wstring _debugString;
		_debugString = MakeMatrixToString(m_matrixWorld);
		return _debugString;
	}

	const std::wstring& Transform::LocalMatrixString() const
	{
		static std::wstring _debugString;
		_debugString = MakeMatrixToString(m_matrixLocal);
		return _debugString;
	}

#endif // DEBUG


}

