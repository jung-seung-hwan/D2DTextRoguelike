#include "pch.h"
#include "D2DApp.h" 
#include "TextRenderer.h"
#include "Scenemanager.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "DataManager.h"
#include "ResourceManager.h"

bool D2DApp::OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    MSG message{};
    message.hwnd = hwnd;
    message.message = msg;
    message.wParam = wparam;
    message.lParam = lparam;

    return InputManager::Instance().OnHandleMessage(message);
}

bool D2DApp::Initialize()
{
    const wchar_t* className = L"D2DTextRoguelike";
    const wchar_t* windowName = L"TextRoguelike";

    if (false == __super::Create(className, windowName, 900, 700))
    {
        return false;
    }

    InputManager::Instance().Initialize(m_hWnd);

    m_pRenderer = myspace::D2DRenderer::CreateD2DRenderer();
    m_pRenderer->Initialize(m_hWnd);

    // 텍스트 분리 처리를 위해 context를 TextRenderer에 넘겨줌
    m_pTextRenderer = new TextRenderer();
    m_pTextRenderer->Initialize(m_pRenderer->GetContext());

    // 배경 로딩
    if (!ResourceManager::Instance().Initialize(m_pRenderer->GetContext()))
    {
        OutputDebugStringW(L"[Error] ResourceManager Initialization Failed.\n");
        return false;
    }
    ResourceManager::Instance().LoadWICBitmap(L"TitleBG", L"./Resource/TitleScene.png");
    ResourceManager::Instance().LoadWICBitmap(L"Player", L"./Resource/Player.png");
    ResourceManager::Instance().LoadWICBitmap(L"Slime", L"./Resource/Slime.png");


    m_timer.Reset();

    DataManager::Instance().LoadMonsterData("./Resource/MonsterData.json");

    // 씬 등록 및 초기 화면 설정 (필요한 씬이 있으면 추가)
    SceneManager::Instance().AddScene(L"TitleScene", new TitleScene());
    SceneManager::Instance().AddScene(L"PlayScene", new PlayScene());

    // 시작 화면을 TitleScene으로 지정
    SceneManager::Instance().ChangeScene(L"TitleScene");

    return true;
}

void D2DApp::Run()
{
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
        }
    }
}

void D2DApp::Finalize()
{
    // 관리자 해제
    SceneManager::Instance().Release();
    ResourceManager::Instance().Release();
    DataManager::Instance().Release();

    if (m_pTextRenderer != nullptr)
    {
        m_pTextRenderer->Release();
        delete m_pTextRenderer;
        m_pTextRenderer = nullptr;
    }

    if (m_pRenderer != nullptr)
    {
        m_pRenderer->Uninitialize();
        delete m_pRenderer;
        m_pRenderer = nullptr;
    }

    __super::Destroy();

    // com 라이브러리 종료
    CoUninitialize();
}

void D2DApp::Update()
{
    m_timer.Tick();
    float deltaTime = m_timer.DeltaTime();

    // 씬 매니저에게 논리 갱신 위임
    SceneManager::Instance().Update(deltaTime);

    // 입력 상태 초기화
    InputManager::Instance().Update();
}

void D2DApp::Render()
{
    RenderFrame();
}

void D2DApp::OnResize(int width, int height)
{
    __super::OnResize(width, height);
}

void D2DApp::OnClose()
{

}

void D2DApp::RenderFrame()
{
    if (!m_pRenderer || !m_pTextRenderer) return;

    m_pRenderer->RenderBegin();

    SceneManager::Instance().Render(
        m_pRenderer,
        m_pTextRenderer
    );

    m_pRenderer->RenderEnd();
}