// TextRoguelike 용 별도 엔진 파일
// 텍스트 관련 변경사항 -> 폰트변경, 특정 텍스트 연출을 해당 코드에서 관리
#include "pch.h"
#include "TextRenderer.h"

TextRenderer::~TextRenderer()
{
    Release();
}

bool TextRenderer::Initialize(ID2D1DeviceContext7* pContext)
{
    if (pContext == nullptr) return false;
    m_pContext = pContext;

    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(m_pDWriteFactory.GetAddressOf())
    );

    if (FAILED(hr)) return false;

    // 기본 폰트 설정
    SetDefaultFont(L"Malgun Gothic", 15.0f);

    return true;
}

void TextRenderer::Release()
{
    m_pTextBrush.Reset();
    m_pDefaultTextFormat.Reset();
    m_pDWriteFactory.Reset();
    m_pContext = nullptr;
}

bool TextRenderer::CreateTextFormat(const std::wstring& fontName, float fontSize)
{
    if (m_pDWriteFactory == nullptr) return false;

    m_pDefaultTextFormat.Reset();

    HRESULT hr = m_pDWriteFactory->CreateTextFormat(
        fontName.c_str(),
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"ko-KR",
        &m_pDefaultTextFormat
    );

    if (FAILED(hr)) return false;

    m_pDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_pDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    m_pDefaultTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

    return true;
}

void TextRenderer::SetDefaultFont(const std::wstring& fontName, float fontSize)
{
    CreateTextFormat(fontName, fontSize);
}

void TextRenderer::DrawText(const std::wstring& text, float x, float y, float width, float height, const D2D1::ColorF& color)
{
    if (m_pContext == nullptr || m_pDefaultTextFormat == nullptr) return;

    if (m_pTextBrush == nullptr)
    {
        m_pContext->CreateSolidColorBrush(color, &m_pTextBrush);
    }
    else
    {
        m_pTextBrush->SetColor(color);
    }

    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + width, y + height);

    m_pContext->DrawTextW(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        m_pDefaultTextFormat.Get(),
        layoutRect,
        m_pTextBrush.Get(),
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
    );
}

D2D1_SIZE_F TextRenderer::MeasureText(const std::wstring& text)
{
    // 향후 UI_Panel이나 UI_Button 크기를 텍스트 길이에 맞춰 자동 조절
    return D2D1::SizeF(0.0f, 0.0f);
}