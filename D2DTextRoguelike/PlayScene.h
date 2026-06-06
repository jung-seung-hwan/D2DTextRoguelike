#pragma once
#include <vector>
#include <memory>
#include "Scene.h"
#include "TextRenderer.h"
#include "DataManager.h"
#include "UIObject.h"
#include "UIButton.h"
#include "DialoguePanel.h"
#include "Player.h"
#include "Monster.h"
#include "CombatManager.h"
#include <functional>

class PlayScene : public Scene
{
public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;
    void Release() override;

private:
    void StartBattle(const std::wstring& monsterId);
    void GoNextFloor();

    void CreateUI();
    void CreateDialoguePanel();
    void CreateButton(
        const std::wstring& text,
        float x,
        float y,
        std::function<void()> onClick
    );

private:
    int m_currentFloor = 1;

    std::vector<std::unique_ptr<UIObject>> m_uiList;
    const MonsterData* m_currentMonster = nullptr;

    Player m_player;
    Monster m_monster;
    CombatManager m_combatManager;

    // 패널 조작을 위한 포인터
    DialoguePanel* m_dialoguePanel = nullptr;

};