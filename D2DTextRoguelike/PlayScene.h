#pragma once
#include "Scene.h"
#include "Player.h"
#include "ISceneState.h"

#include <vector>
#include <memory>

class PlayScene : public Scene
{
public:
    PlayScene() = default;
    ~PlayScene() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) override;
    void Release() override;

    // 상태를 전환하는 인터페이스
    void ChangeState(std::unique_ptr<ISceneState> newState);

    // 상태 객체들이 씬의 데이터를 참조하거나 조작하기 위한 인터페이스
    Player* GetPlayer() { return &m_player; }
    int GetCurrentFloor() const { return m_currentFloor; }
    void IncreaseFloor() { m_currentFloor++; }

private:
    // 씬에서 공통으로 유지해야 할 데이터
    int m_currentFloor = 1;
    Player m_player;

    // 현재 실행 중인 상태를 관리하는 스마트 포인터
    std::unique_ptr<ISceneState> m_currentState;
    std::unique_ptr<ISceneState> m_nextState;
};