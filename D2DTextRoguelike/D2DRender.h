#pragma once

#include <wrl/client.h>
#include <d2d1helper.h>
using namespace Microsoft::WRL;

namespace myspace
{
    class D2DRenderer
    {
    public:
        D2DRenderer() = default;

        ~D2DRenderer() { Uninitialize(); }

        ID2D1DeviceContext7* GetContext() const
        {
            return m_d2dContext.Get();
        }

        static D2DRenderer* CreateD2DRenderer();

        void Initialize(HWND hwnd);

        void Uninitialize();

        void Resize(UINT width, UINT height);

        void DrawLine(float x1, float y1, float x2, float y2, const D2D1::ColorF& color);

        void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color);

        void DrawRectangle(float left, float top, float right, float bottom, const D2D1::ColorF& color);

        void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest);

        void SetTransform(const D2D1_MATRIX_3X2_F tm);

        void RenderBegin();

        void RenderEnd();

        // 텍스트 상자용
        void FillRectangle(const D2D1_RECT_F& rect, const D2D1::ColorF& color);
        void DrawRectangle(const D2D1_RECT_F& rect, const D2D1::ColorF& color, float thickness = 1.0f);

        // fade in, fade out
        D2D1_SIZE_F GetSize() const
        {
            return m_d2dContext->GetSize();
        }

        void DrawBitmap(
            ID2D1Bitmap* bitmap,
            const D2D1_RECT_F& dest);

        void DrawHPBar(
            float x,
            float y,
            float width,
            float height,
            int hp,
            int maxHp);

    private:

        void CreateDeviceAndSwapChain(HWND hwnd);

        void CreateRenderTargets();

        void ReleaseRenderTargets();

    private:
        HWND m_hwnd = nullptr;

        ComPtr<ID3D11Device>           m_d3dDevice;
        ComPtr<IDXGISwapChain1>        m_swapChain;

        ComPtr<ID2D1Bitmap1>           m_targetBitmap; // D2D render target bitmap
        ComPtr<ID2D1SolidColorBrush>   m_brush;

        ComPtr<ID2D1Device7>           m_d2dDevice;
        ComPtr<ID2D1DeviceContext7>    m_d2dContext;

        ComPtr<IWICImagingFactory>     m_wicFactory;
    };
}// namespace 
