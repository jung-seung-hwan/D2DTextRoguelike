#include "pch.h"
#include "PlayerInfoPanel.h"

#include "D2DRender.h"
#include "TextRenderer.h"
#include "ResourceManager.h"

#include "Inventory.h"
#include "Item.h"

PlayerInfoPanel::PlayerInfoPanel(Player* player, float width, float height)
    : m_player(player)
{
    m_size = { width, height };
}


void PlayerInfoPanel::Update(float deltaTime)
{
    UIObject::Update(deltaTime);
}


void PlayerInfoPanel::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (!m_isActive || !m_isVisible || m_player == nullptr) return;

    // 플레이어 스탯 텍스트 구성
    wchar_t statBuffer[256];
    swprintf_s(statBuffer, 256,
        L"[ 플레이어 정보 ]\n\n"
        L"체력: %d / %d\n"
        L"공격력: %d\n"
        L"방어력: %d\n"
        L"회피율: %d%%\n\n"
        L"[ 보유 아이템 ]",
        m_player->hp, m_player->maxHp,
        m_player->attack, m_player->defense, m_player->evasion);

    // 스탯 텍스트 렌더링
    pTextRenderer->DrawText(
        statBuffer,
        m_globalPosition.x + 20.0f, m_globalPosition.y + 20.0f,
        m_size.width - 40.0f, 200.0f,
        D2D1::ColorF(D2D1::ColorF::White),
        TextAlign::Left, VerticalAlign::Top
    );

    // 아이템 목록 렌더링
    float itemStartY = m_globalPosition.y + 200.0f;
    const auto& equipmentMap = m_player->GetInventory().GetEquipment();

    int drawIndex = 0; // Y축 위치
    for (const auto& pair : equipmentMap)
    {
        wchar_t itemBuffer[128];
        const auto& item = pair.second;

        // 아이템 이름과 현재 강화 단계
        swprintf_s(itemBuffer, 128, L"- %s (+%d)",
            item->GetName().c_str(),
            item->GetEnhanceLevel());

        pTextRenderer->DrawText(
            itemBuffer,
            m_globalPosition.x + 20.0f, itemStartY + (drawIndex * 30.0f),
            m_size.width - 40.0f, 30.0f,
            D2D1::ColorF(D2D1::ColorF::LightGray),
            TextAlign::Left, VerticalAlign::Top
        );

        drawIndex++; // 다음 항목의 출력 위치를 위해 인덱스 증가
    }
}