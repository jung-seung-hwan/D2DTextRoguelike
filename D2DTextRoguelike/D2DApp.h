#pragma once

#include "NzWndBase.h"
#include "InputManager.h"
#include "TextRenderer.h"
#include "GameTimer.h"
#include "SceneManager.h"

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class D2DApp : public NzWndBase
{
public:
    D2DApp() = default;
    virtual ~D2DApp() = default;

    bool Initialize();
    void Run();
    void Finalize();

private:
    void Update();
    void Render();

    void OnResize(int width, int height) override;
    void OnClose() override;

    bool InitD2DRenderSystem(HWND hwnd);
    void ReleaseD2DRenderSystem();
    void RenderFrame();


    // D3D11
    ComPtr<ID3D11Device>           m_d3dDevice;
    ComPtr<ID3D11DeviceContext>    m_d3dContext;

    // DXGI
    ComPtr<IDXGISwapChain1>        m_swapChain;

    // Direct2D 10 인터페이스
    //ComPtr<ID2D1Factory4>          m_d2dFactory;
    //ComPtr<ID2D1Device4>           m_d2dDevice;
    //ComPtr<ID2D1DeviceContext4>    m_d2dContext;
    // Direct2D 11 최신 인터페이스
    // ComPtr<ID2D1Factory8>          m_d2dFactory;
    ComPtr<ID2D1Device7>           m_d2dDevice;
    ComPtr<ID2D1DeviceContext7>    m_d2dContext; // SpriteBatch 


    // Direct2D 이전 인터페이스
    ComPtr<ID2D1HwndRenderTarget> m_renderTarget;

    // Background
    ComPtr<ID2D1SolidColorBrush> m_brush;

    // Offscreen RenderTarget
    ComPtr<ID2D1Bitmap1>          m_targetBitmap;

    TextRenderer* m_pTextRenderer = nullptr;

    // 타이머 사용
    GameTimer m_timer;
protected:
    bool OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
};
