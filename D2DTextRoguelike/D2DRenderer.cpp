#include "pch.h"
#include "D2DRender.h"

using namespace myspace;

D2DRenderer* D2DRenderer::CreateD2DRenderer()
{
    static D2DRenderer* instance = new D2DRenderer;

    if (nullptr == instance)
    {
        throw std::runtime_error("Failed to create D2DRenderer instance.");
    }
    return (instance);
}

void D2DRenderer::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;

    CreateDeviceAndSwapChain(hwnd);
    CreateRenderTargets();

}

void D2DRenderer::Uninitialize()
{
    ReleaseRenderTargets();

    m_targetBitmap = nullptr;
    m_brush = nullptr;

    m_d2dContext = nullptr;
    m_d2dDevice = nullptr;

    m_swapChain = nullptr;
    m_d3dDevice = nullptr;
}

void D2DRenderer::Resize(UINT width, UINT height)
{
    ReleaseRenderTargets();

    // 스왑체인 크기 조정 후 렌더 타겟 재생성
    DX::ThrowIfFailed(m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));

    CreateRenderTargets();
}

void D2DRenderer::DrawLine(float x1, float y1, float x2, float y2, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), m_brush.Get());
}

void D2DRenderer::DrawCircle(float x, float y, float radius, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), m_brush.Get());
}

void D2DRenderer::DrawRectangle(float left, float top, float right, float bottom, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawRectangle(D2D1::Rect(left, top, right, bottom), m_brush.Get());
}

void D2DRenderer::DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest)
{
    m_d2dContext->DrawBitmap(bitmap, dest);
}


// 엔진 분리 void D2DRenderer::DrawMessage - D2DTextRoguelike에서 별도로 분리해서 사용

void D2DRenderer::SetTransform(const D2D1_MATRIX_3X2_F tm)
{
    if (m_d2dContext) m_d2dContext->SetTransform(tm);
}

void D2DRenderer::RenderBegin()
{
    m_d2dContext->BeginDraw();
    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black)); // 배경을 흰색으로 초기화
}

void D2DRenderer::RenderEnd()
{
    m_d2dContext->EndDraw();

    // 렌더링 작업이 끝나면 스왑체인에 프레임을 표시
    HRESULT hr = m_swapChain->Present(1, 0);

    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        Uninitialize();     // 디바이스가 제거되거나 리셋된 경우, 재초기화 필요
        Initialize(m_hwnd);
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

void D2DRenderer::CreateDeviceAndSwapChain(HWND hwnd)
{
    //1. D3D11 디바이스 생성
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    ComPtr<ID3D11Device> d3dDevice;

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
        nullptr);                       //[out, optional] ID3D11DeviceContext** ppImmediateContext

    DX::ThrowIfFailed(hr);

    // 2. DXGI 스왑체인 생성
    ComPtr<IDXGIDevice> dxgiDevice;
    hr = d3dDevice.As(&dxgiDevice);

    DX::ThrowIfFailed(hr);

    ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);

    DX::ThrowIfFailed(hr);

    ComPtr<IDXGIFactory2> dxgiFactory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

    DX::ThrowIfFailed(hr);

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

    DX::ThrowIfFailed(hr);

    // 3. ID2D1Factory4 생성
    D2D1_FACTORY_OPTIONS opts = {};
    ComPtr<ID2D1Factory8> d2dFactory;

#if defined(_DEBUG)
    opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory8),
        &opts,
        reinterpret_cast<void**>(d2dFactory.GetAddressOf()));

    DX::ThrowIfFailed(hr);

    // 4. ID2D1Device4 생성
    ComPtr<ID2D1Device> baseDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);

    DX::ThrowIfFailed(hr);

    ComPtr<ID2D1Device7> d2dDevice;
    hr = baseDevice.As(&d2dDevice);

    DX::ThrowIfFailed(hr);

    // 5. ID2D1DeviceContext7 생성
    ComPtr<ID2D1DeviceContext7> d2dContext;//
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);

    DX::ThrowIfFailed(hr);

    m_d3dDevice = d3dDevice;
    m_swapChain = swapChain;

    m_d2dDevice = d2dDevice;
    m_d2dContext = d2dContext;
}

void D2DRenderer::CreateRenderTargets()
{
    // 6. SwapChain 백버퍼 -> D2D Bitmap1 을 생성하여 렌더 타겟으로 설정

    ComPtr<IDXGISurface> dxgiSurface;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));

    DX::ThrowIfFailed(hr);

    D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
    bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProps.dpiX = bitmapProps.dpiY = 96.0f;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = m_d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, targetBitmap.GetAddressOf());

    DX::ThrowIfFailed(hr);

    // 렌더 타겟 설정
    m_d2dContext->SetTarget(targetBitmap.Get());

    m_targetBitmap = targetBitmap;

    hr = m_d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Blue),
        &m_brush);

    DX::ThrowIfFailed(hr);
}


// 엔진 분리 void D2DRenderer::CreateWriteResource() - D2DTextRoguelike에서 별도로 분리해서 사용
void D2DRenderer::ReleaseRenderTargets()
{
    if (m_d2dContext)
    {
        m_d2dContext->SetTarget(nullptr);
    }

    m_targetBitmap.Reset();
    m_brush.Reset();
}

void D2DRenderer::FillRectangle(const D2D1_RECT_F& rect, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->FillRectangle(rect, m_brush.Get());
}

void D2DRenderer::DrawRectangle(const D2D1_RECT_F& rect, const D2D1::ColorF& color, float thickness)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawRectangle(rect, m_brush.Get(), thickness);
}

void D2DRenderer::DrawHPBar(
    float x,
    float y,
    float width,
    float height,
    int hp,
    int maxHp)
{
    if (maxHp <= 0) return;

    float ratio = static_cast<float>(hp) / static_cast<float>(maxHp);
    ratio = std::max(0.0f, std::min(ratio, 1.0f));

    D2D1_COLOR_F hpColor;
    D2D1_COLOR_F hpShadowColor;

    if (ratio > 0.5f)
    {
        hpColor = D2D1::ColorF(0.1f, 0.9f, 0.2f);
        hpShadowColor = D2D1::ColorF(0.05f, 0.45f, 0.1f);
    }
    else if (ratio > 0.25f)
    {
        hpColor = D2D1::ColorF(1.0f, 0.55f, 0.05f);
        hpShadowColor = D2D1::ColorF(0.5f, 0.25f, 0.02f);
    }
    else
    {
        hpColor = D2D1::ColorF(0.95f, 0.1f, 0.1f);
        hpShadowColor = D2D1::ColorF(0.45f, 0.02f, 0.02f);
    }

    D2D1_ROUNDED_RECT bgRect =
    {
        D2D1::RectF(x, y, x + width, y + height),
        6.0f,
        6.0f
    };

    m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
    m_d2dContext->FillRoundedRectangle(bgRect, m_brush.Get());

    float hpLeft = x + 2.0f;
    float hpTop = y + 2.0f;
    float hpRight = x + 2.0f + (width - 4.0f) * ratio;
    float hpBottom = y + height - 2.0f;

    D2D1_ROUNDED_RECT hpRect =
    {
        D2D1::RectF(hpLeft, hpTop, hpRight, hpBottom),
        4.0f,
        4.0f
    };

    m_brush->SetColor(hpShadowColor);
    m_d2dContext->FillRoundedRectangle(hpRect, m_brush.Get());

    D2D1_RECT_F hpHighlightRect =
        D2D1::RectF(hpLeft, hpTop, hpRight, hpTop + (hpBottom - hpTop) * 0.65f);

    m_brush->SetColor(hpColor);
    m_d2dContext->FillRectangle(hpHighlightRect, m_brush.Get());

    m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    m_d2dContext->DrawRoundedRectangle(bgRect, m_brush.Get(), 2.0f);
}


// ResourceManager에서 처리
void D2DRenderer::DrawBitmap(
    ID2D1Bitmap* bitmap,
    const D2D1_RECT_F& dest)
{
    if (bitmap == nullptr) return;

    m_d2dContext->DrawBitmap(bitmap, dest);
}