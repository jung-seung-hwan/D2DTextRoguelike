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

    // 패널을 좌우 두 구역으로 나누기 위한 기준 너비 계산
    float leftWidth = m_size.width * 0.3f;
    float rightWidth = m_size.width * 0.7f;
    float leftColumnX = m_globalPosition.x + 20.0f;
    float rightColumnX = m_globalPosition.x + leftWidth + 20.0f;

    // 플레이어 스탯 텍스트 구성
    wchar_t statBuffer[256];
    swprintf_s(statBuffer, 256,
        L"[ 플레이어 정보 ]\n\n"
        L"체력: %d / %d\n"
        L"공격력: %d\n"
        L"방어력: %d\n"
        L"회피율: %d%%",
        m_player->hp, m_player->maxHp,
        m_player->attack, m_player->defense, m_player->evasion);

    // 좌측 영역에 스탯 텍스트 렌더링
    pTextRenderer->DrawText(
        statBuffer,
        leftColumnX, m_globalPosition.y + 20.0f,
        leftWidth - 40.0f, 200.0f,
        D2D1::ColorF(D2D1::ColorF::White),
        TextAlign::Left, VerticalAlign::Top
    );

    // 우측 영역에 아이템 목록 헤더 렌더링
    pTextRenderer->DrawText(
        L"[ 보유 아이템 ]",
        rightColumnX, m_globalPosition.y + 20.0f,
        rightWidth - 10.0f, 30.0f,
        D2D1::ColorF(D2D1::ColorF::White),
        TextAlign::Left, VerticalAlign::Top
    );

    // 우측 영역에 아이템 목록 렌더링
    float itemStartY = m_globalPosition.y + 60.0f; 
    const auto& equipmentMap = m_player->GetInventory().GetEquipment();

    int drawIndex = 0; // Y축 위치
    for (const auto& pair : equipmentMap)
    {
        wchar_t itemBuffer[128];
        const auto& item = pair.second;

        // 아이템의 능력치 타입을 화면 출력용 한글 문자열로 변환
        std::wstring statName = L"";
        switch (item->GetStatType())
        {
        case ItemStatType::Attack:  statName = L"공격력"; break;
        case ItemStatType::Defense: statName = L"방어력"; break;
        case ItemStatType::Evasion: statName = L"회피율"; break;
        case ItemStatType::Hp:      statName = L"최대 체력"; break;
        }

        // 능력치 종류, 능력치 수치, 아이템 이름, 강화 단계
        swprintf_s(itemBuffer, 128, L"- [%s +%d] %s (+%d)",
            statName.c_str(),
            item->GetStatValue(),
            item->GetName().c_str(),
            item->GetEnhanceLevel());

        // 우측 컬럼 X좌표(rightColumnX)를 기준으로 렌더링
        pTextRenderer->DrawText(
            itemBuffer,
            rightColumnX, itemStartY + (drawIndex * 30.0f),
            rightWidth - 10.0f, 30.0f,
            D2D1::ColorF(D2D1::ColorF::LightGray),
            TextAlign::Left, VerticalAlign::Top
        );

        drawIndex++; // 다음 항목의 출력 위치를 위해 인덱스 증가
    }
}