#pragma once
#include "ISceneState.h"
#include "Monster.h"
#include "CombatManager.h"
#include "UIObject.h"

class Player;
class DialoguePanel;
class UIButton;
class DiceRollPanel;
struct ItemData;


class CombatState : public ISceneState
{
public:
    CombatState(int floor, MonsterType type);
    virtual ~CombatState() = default;

    virtual void Enter(PlayScene* pScene) override;
    virtual void Update(PlayScene* pScene, float deltaTime) override;
    virtual void Render(PlayScene* pScene, myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;
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
    DiceRollPanel* m_diceRollPanel = nullptr;

    // 패배 메시지 확인 여부를 저장하는 논리 변수 추가
    bool m_isDefeatAcknowledged = false;

    bool m_showEnemyDice = false;
    int m_enemyDiceNumber = 0;
    float m_enemyDiceTimer = 0.0f;
    float m_enemyDiceDisplayTime = 1.0f;

    // 보상 처리 및 화면 제어용 포인터 변수 추가
    const ItemData* m_droppedItemData = nullptr;
    UIButton* m_equipBtn = nullptr;
    UIButton* m_discardBtn = nullptr;
    UIButton* m_nextBtn = nullptr;
    UIButton* m_attackBtn = nullptr;
    UIButton* m_defendBtn = nullptr;
    UIButton* m_diceBtn = nullptr;

    void CompleteRewardSelection();

};