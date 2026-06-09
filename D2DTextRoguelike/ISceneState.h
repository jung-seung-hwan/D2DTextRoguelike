#pragma once
#include "D2DRender.h"
// 전방 선언: 상태 객체가 씬의 상태를 변경하거나 데이터를 참조할 수 있도록 PlayScene 포인터를 전달받음
class PlayScene;
struct ID2D1DeviceContext7;
class TextRenderer;


class ISceneState
{
public:
    virtual ~ISceneState() = default; // 다형성을 위한 가상 소멸자 필수

    // 상태 진입 시 1회 호출 (초기화 로직)
    virtual void Enter(PlayScene* pScene) = 0;

    // 매 프레임 호출 (로직 갱신)
    virtual void Update(PlayScene* pScene, float deltaTime) = 0;

    // 매 프레임 호출 (화면 출력)
    virtual void Render(PlayScene* pScene, myspace::D2DRenderer* pRenderer, TextRenderer* pTextRenderer) = 0;

    // 상태 종료 시 1회 호출 (메모리 해제 및 정리 로직)
    virtual void Exit(PlayScene* pScene) = 0;
};