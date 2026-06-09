#pragma once
#include "UIObject.h"
#include "GameObject.h"


class StatusPanel : public UIObject
{
public:
	StatusPanel(GameObject* target, float width, float height);

	void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;

private:
	void DrawName(TextRenderer* pTextRenderer);
	void DrawHPBar(ID2D1DeviceContext7* pContext);
	void DrawStats(TextRenderer* pTextRenderer);

private:
	GameObject* m_target = nullptr;

	float m_width = 0.0f;
	float m_height = 0.0f;
};

