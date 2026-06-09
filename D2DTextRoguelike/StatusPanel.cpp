#include "pch.h"
#include "StatusPanel.h"
#include "TextRenderer.h"

StatusPanel::StatusPanel(GameObject* target, float width, float height)
	:m_target(target), m_width(width), m_height(height)
{
}

void StatusPanel::Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer)
{
	if (m_target == nullptr || m_target->maxHp <= 0)
		return;

	DrawName(pTextRenderer);
	DrawHPBar(pContext);
	DrawStats(pTextRenderer);
}



void StatusPanel::DrawName(TextRenderer* pTextRenderer)
{
	float x = m_localPosition.x;
	float y = m_localPosition.y;

	pTextRenderer->DrawText(
		m_target->name.c_str(),
		x,
		y - 28.0f,
		x + m_width,
		y,
		D2D1::ColorF(D2D1::ColorF::White)
	);
}


void StatusPanel::DrawHPBar(ID2D1DeviceContext7* pContext)
{
	if (m_target == nullptr || m_target->maxHp <= 0)
		return;

	float ratio = static_cast<float>(m_target->hp) / m_target->maxHp;

	if (ratio < 0.0f) ratio = 0.0f;
	if (ratio > 1.0f) ratio = 1.0f;

	D2D1_COLOR_F hpColor;
	D2D1_COLOR_F hpShadowColor;

	if (ratio > 0.5f)			// HP가 50%를 넘으면 녹색
	{
		hpColor = D2D1::ColorF(0.1f, 0.9f, 0.2f);
		hpShadowColor = D2D1::ColorF(0.05f, 0.45f, 0.1f);
	}
	else if (ratio > 0.25f)		// HP가 26% - 50% 면 주황색
	{
		hpColor = D2D1::ColorF(1.0f, 0.55f, 0.05f);
		hpShadowColor = D2D1::ColorF(0.5f, 0.25f, 0.02f);
	}
	else						// HP가 25% 이하면 빨간색
	{
		hpColor = D2D1::ColorF(0.95f, 0.1f, 0.1f);
		hpShadowColor = D2D1::ColorF(0.45f, 0.02f, 0.02f);
	}

	float x = m_localPosition.x;
	float y = m_localPosition.y;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bgBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> hpBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> hpShadowBrush;		// HP바 그림자색

	pContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &bgBrush);
	pContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &borderBrush);
	pContext->CreateSolidColorBrush(hpColor, &hpBrush);
	pContext->CreateSolidColorBrush(hpShadowColor, &hpShadowBrush);

	D2D1_ROUNDED_RECT roundedRect =
	{
		D2D1::RectF(x, y, x + m_width, y + m_height),
		6.0f, // radiusX
		6.0f  // radiusY
	};


	pContext->FillRoundedRectangle(
		roundedRect,
		bgBrush.Get()
	);

	float hpLeft = x + 2;
	float hpTop = y + 2;
	float hpRight = x + 2 + (m_width - 4) * ratio;
	float hpBottom = y + m_height - 2;

	float hpMidY = hpTop + (hpBottom - hpTop) * 0.9f;

	D2D1_ROUNDED_RECT hpRect =
	{
		D2D1::RectF(
			hpLeft,
			hpTop,
			hpRight,
			hpBottom
		),
		4.0f,
		4.0f
	};

	// 체력바의 그림자
	pContext->FillRoundedRectangle(
		hpRect,
		hpShadowBrush.Get()
	);

	// 체력바 원래 색상
	pContext->FillRoundedRectangle(
		hpRect,
		hpBrush.Get()
	);

	pContext->DrawRoundedRectangle(
		roundedRect,
		borderBrush.Get(),
		2.0f
	);
}



void StatusPanel::DrawStats(TextRenderer* pTextRenderer)
{
	float x = m_localPosition.x;
	float y = m_localPosition.y;

	wchar_t buffer[128];

	swprintf_s(
		buffer,
		128,
		L"%d/%d",
		m_target->hp,
		m_target->maxHp
	);

	pTextRenderer->DrawText(
		buffer,
		x + m_width - 50.0f,
		y + m_height + 6.0f,
		x + m_width,
		y + m_height + 22.0f,
		D2D1::ColorF(D2D1::ColorF::White)
	);
}