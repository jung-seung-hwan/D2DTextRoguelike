#pragma once
#include "PlayScene.h"
#include "ISceneState.h"
#include "CombatManager.h"
#include "Monster.h"
#include "UIObject.h"
#include "DialoguePanel.h"
#include "UIButton.h"
#include <vector>
#include <memory>
#include "Player.h"

class CombatState : public ISceneState
{
public:
    CombatState(int floor, MonsterType type);
    virtual ~CombatState() = default;

    virtual void Enter(PlayScene* pScene) override;
    virtual void Update(PlayScene* pScene, float deltaTime) override;
    virtual void Render(PlayScene* pScene, ID2D1DeviceContext7* pContext, TextRenderer* pTextRenderer) override;
    virtual void Exit(PlayScene* pScene) override;

private:
    void StartCombat(PlayScene* pScene);
    void CreateUI(PlayScene* pScene);

private:
    int m_floor;
    Player* m_player = nullptr;
    MonsterType m_type;
    Monster m_monster;
    CombatManager m_combatManager;

    // 전투 상태에서만 사용하는 고유 UI 리스트
    std::vector<std::unique_ptr<UIObject>> m_uiList;
    DialoguePanel* m_dialoguePanel = nullptr;
};