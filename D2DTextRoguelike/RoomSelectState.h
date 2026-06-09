#pragma once
#include "PlayScene.h"
#include "ISceneState.h"
#include "UIObject.h"
#include "DialoguePanel.h"
#include "UIButton.h"
#include <vector>
#include <memory>

class RoomSelectState : public ISceneState
{
public:
    RoomSelectState() = default;
    virtual ~RoomSelectState() = default;

    virtual void Enter(PlayScene* pScene) override;
    virtual void Update(PlayScene* pScene, float deltaTime) override;
    virtual void Render(PlayScene* pScene, myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;
    virtual void Exit(PlayScene* pScene) override;

private:
    void CreateRoomButtons(PlayScene* pScene);

private:
    // 방 선택 화면에서만 사용할 버튼 UI 컨테이너
    std::vector<std::unique_ptr<UIObject>> m_uiList;
};