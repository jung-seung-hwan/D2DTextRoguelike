#pragma once
#include "ISceneState.h"
#include "UIObject.h"

#include <vector>
#include <memory>

class Player;
class DialoguePanel;

class RestState : public ISceneState
{
public:
    RestState(int floor);
    virtual ~RestState() = default;

    virtual void Enter(PlayScene* pScene) override;
    virtual void Update(PlayScene* pScene, float deltaTime) override;
    virtual void Render(PlayScene* pScene, myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;
    virtual void Exit(PlayScene* pScene) override;

private:
    void CreateUI(PlayScene* pScene);

private:
    int m_floor;
    Player* m_player = nullptr;
    std::vector<std::unique_ptr<UIObject>> m_uiList;
    DialoguePanel* m_dialoguePanel = nullptr;
};