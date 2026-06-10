#include "pch.h"
#include "StatusPanel.h"

#include "D2DRender.h"
#include "TextRenderer.h"

StatusPanel::StatusPanel(GameObject* target, float width, float height)
	:m_target(target), m_width(width), m_height(height)
{
}

void StatusPanel::Render(myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer)
{
	if (m_target == nullptr || m_target->maxHp <= 0)
		return;

	DrawName(pTextRenderer);
	DrawHPBar(m_pRenderer);
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


void StatusPanel::DrawHPBar(myspace::D2DRenderer* m_pRenderer)
{
	m_pRenderer->DrawHPBar(
		m_localPosition.x,
		m_localPosition.y,
		m_width,
		m_height,
		m_target->hp,
		m_target->maxHp
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