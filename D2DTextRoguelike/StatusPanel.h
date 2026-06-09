#pragma once
#include "UIObject.h"
#include "GameObject.h"


class StatusPanel : public UIObject
{
public:
	StatusPanel(GameObject* target, float width, float height);

	void Render(myspace::D2DRenderer* m_pRenderer, TextRenderer* pTextRenderer) override;

private:
	void DrawName(TextRenderer* pTextRenderer);
	void DrawHPBar(myspace::D2DRenderer* m_pRenderer);
	void DrawStats(TextRenderer* pTextRenderer);

private:
	GameObject* m_target = nullptr;

	float m_width = 0.0f;
	float m_height = 0.0f;
};

