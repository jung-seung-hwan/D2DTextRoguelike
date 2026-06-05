#include "pch.h"
#include "D2DApp.h" 
#include "TextRenderer.h"
#include "Scenemanager.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "DataManager.h"

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

    if (false == __super::Create(className, windowName, 430, 932))
    {
        return false;
    }

    InputManager::Instance().Initialize(m_hWnd);
    InitD2DRenderSystem(m_hWnd);

    // 텍스트 분리 처리를 위해 context를 TextRenderer에 넘겨줌
    m_pTextRenderer = new TextRenderer();
    m_pTextRenderer->Initialize(m_d2dContext.Get());

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
    if (m_pTextRenderer != nullptr)
    {
        m_pTextRenderer->Release();
        delete m_pTextRenderer;
        m_pTextRenderer = nullptr;
    }

    ReleaseD2DRenderSystem();

    __super::Destroy();

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
    if (!m_d2dContext || !m_swapChain) return;

    m_d2dContext->BeginDraw();

    // 배경 클리어
    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    // 씬 매니저에게 화면 출력 위임
    SceneManager::Instance().Render(m_d2dContext.Get(), m_pTextRenderer);

    m_d2dContext->EndDraw();
    m_swapChain->Present(1, 0);
}


bool D2DApp::InitD2DRenderSystem(HWND hwnd)
{
    //1. D3D11 디바이스 생성
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    ComPtr<ID3D11Device> d3dDevice;
    ComPtr<ID3D11DeviceContext> d3dContext;

    HRESULT hr = D3D11CreateDevice(
        nullptr,                            //[in, optional]  IDXGIAdapter* pAdapter
        D3D_DRIVER_TYPE_HARDWARE,           //D3D_DRIVER_TYPE DriverType
        nullptr,                            //HMODULE         Software
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,   //UINT            Flags
        featureLevels,                      //[in, optional]  const D3D_FEATURE_LEVEL* pFeatureLevels
        ARRAYSIZE(featureLevels),           //UINT            FeatureLevels
        D3D11_SDK_VERSION,                  //UINT            SDKVersion
        &d3dDevice,                         //[out, optional] ID3D11Device** ppDevice
        nullptr,                            //[out, optional] D3D_FEATURE_LEVEL* pFeatureLevel
        &d3dContext);                       //[out, optional] ID3D11DeviceContext** ppImmediateContext

    if (FAILED(hr)) return false;

    // 2. DXGI 스왑체인 생성
    ComPtr<IDXGIDevice> dxgiDevice;
    hr = d3dDevice.As(&dxgiDevice);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIFactory2> dxgiFactory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr)) return false;

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scDesc.SampleDesc.Count = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = 2;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Scaling = DXGI_SCALING_STRETCH;

    ComPtr<IDXGISwapChain1> swapChain;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        d3dDevice.Get(), hwnd, &scDesc, nullptr, nullptr, &swapChain);
    if (FAILED(hr)) return false;

    // 3. ID2D1Factory4 생성
    D2D1_FACTORY_OPTIONS opts = {};
    ComPtr<ID2D1Factory8> d2dFactory;

    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory8),
        &opts,
        reinterpret_cast<void**>(d2dFactory.GetAddressOf()));
    if (FAILED(hr)) return false;

    // 4. ID2D1Device4 생성
    ComPtr<ID2D1Device> baseDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);
    if (FAILED(hr)) return false;

    ComPtr<ID2D1Device7> d2dDevice;
    hr = baseDevice.As(&d2dDevice);
    if (FAILED(hr)) return false;

    // 5. ID2D1DeviceContext7 생성
    ComPtr<ID2D1DeviceContext7> d2dContext;//
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
    if (FAILED(hr)) return false;

    // 6. SwapChain 백버퍼 -> D2D Bitmap1 을 생성하여 렌더 타겟으로 설정

    ComPtr<IDXGISurface> dxgiSurface;
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
    bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProps.dpiX = bitmapProps.dpiY = 96.0f;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, targetBitmap.GetAddressOf());
    if (FAILED(hr)) return false;

    // 렌더 타겟 설정
    d2dContext->SetTarget(targetBitmap.Get());

    // 7. 기본 브러시 생성
    ComPtr<ID2D1SolidColorBrush> brush;
    hr = d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Tomato), &brush);
    if (FAILED(hr)) return false;

    // 8. 멤버로 저장
    m_d3dDevice = d3dDevice;
    m_d3dContext = d3dContext;
    m_swapChain = swapChain;
    m_d2dDevice = d2dDevice;
    m_d2dContext = d2dContext;
    m_targetBitmap = targetBitmap;
    m_brush = brush;

    return true;
}

void D2DApp::ReleaseD2DRenderSystem()
{
    m_brush.Reset();
    m_targetBitmap.Reset();

    m_d2dContext.Reset();
    m_d2dDevice.Reset();
    m_d2dDevice.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();
}