#include "pch.h"
#include "PlayScene.h"
#include "RoomSelectState.h"

void PlayScene::Initialize()
{
    m_currentFloor = 1;

    m_player.name = L"플레이어";
    m_player.maxHp = 100;
    m_player.hp = 100;
    m_player.attack = 1;
    m_player.defense = 0;
    m_player.evasion = 10;
    m_player.isDead = false;
    ChangeState(std::make_unique<RoomSelectState>());

}

void PlayScene::Update(float deltaTime)
{
    // 프레임 시작 시, 예약된 상태 전환이 있다면 안전하게 교체
    if (m_nextState)
    {
        if (m_currentState)
        {
            m_currentState->Exit(this);
        }

        m_currentState = std::move(m_nextState);

        if (m_currentState)
        {
            m_currentState->Enter(this);
        }
    }

    // 현재 상태 갱신
    if (m_currentState)
    {
        m_currentState->Update(this, deltaTime);
    }
}

void PlayScene::Render(myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer)
{
    if (m_currentState)
    {
        m_currentState->Render(this, pRenderer, pTextRenderer); // 현재 상태에 렌더링 위임
    }
}

void PlayScene::ChangeState(std::unique_ptr<ISceneState> newState)
{
    // 즉시 교체하지 않고, 다음 프레임에 교체되도록 예약
    m_nextState = std::move(newState);
}

void PlayScene::Release()
{
    if (m_currentState)
    {
        m_currentState->Exit(this);
        m_currentState.reset(); // 스마트 포인터 초기화
    }
}