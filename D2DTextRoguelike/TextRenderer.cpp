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
    // 글씨를 그릴 화면 확인
    if (pContext == nullptr) return false;
    m_pContext = pContext;

    // DirectWrite 객체(TextFormat, TextLayout 등)를 생성하기 위한 Factory 생성
    // 생성된 IDWriteFactory 인터페이스를 m_pDWriteFactory에 저장
    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(
            m_pDWriteFactory.GetAddressOf()
            )
    );

    if (FAILED(hr)) return false;

    // 기본 폰트 설정
    SetDefaultFont(L"맑은 고딕", 15.0f);

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

    // IDWriteTextFormat 객체를 생성
    /* 
        인자1 : 사용할 폰트 이름
        인자2 : Font Collection
        인자3 : 폰트 굵기
        인자4 : 폰트 스타일
        인자5 : 폰트 폭
        인자6 : 글자 크기
        인자7 : 로케일
    */
    IDWriteTextFormat* pRawFormat = nullptr;
    HRESULT hr = m_pDWriteFactory->CreateTextFormat(
        fontName.c_str(),
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"",
        &pRawFormat
    );

    if (FAILED(hr)) return false;

    m_pDefaultTextFormat.Attach(pRawFormat);


    // 문단 정렬 (세로)
    m_pDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    // 자동 줄바꿈
    m_pDefaultTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

    return true;
}

void TextRenderer::SetDefaultFont(const std::wstring& fontName, float fontSize)
{
    CreateTextFormat(fontName, fontSize);
}


/*
* 매개변수 정보
    text : 화면에 출력할 문자열
    x : 텍스트 영역의 시작 X 좌표
    y : 텍스트 영역의 시작 Y 좌표
    width : 텍스트가 출력될 영역의 너비
    height : 텍스트가 출력될 영역의 높이
    color : 텍스트 색상
    align : 정렬 방식
*/
void TextRenderer::DrawText(const std::wstring& text, float x, float y, float width, float height, const D2D1::ColorF& color, TextAlign align) {
    if (m_pContext == nullptr || m_pDefaultTextFormat == nullptr) return;

    // 텍스트를 그릴 색상 브러시 생성
    if (m_pTextBrush == nullptr)
    {
        m_pContext->CreateSolidColorBrush(color, &m_pTextBrush);
    }
    else
    {
        // 기존 브러시가 있으면 새로 브러쉬 만들지 않고 색상만 변경
        m_pTextBrush->SetColor(color);
    }

    // 호출 시 전달받은 정렬 방식에 따라 서식의 속성을 동적으로 변경
    switch (align)
    {
    case TextAlign::Left:
        m_pDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        break;
    case TextAlign::Center:
        m_pDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        break;
    case TextAlign::Right:
        m_pDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
        break;
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

// 현재 폰트가 공간이 얼마나 필요한지 계산 -> 실제 픽셀 크기 계산
D2D1_SIZE_F TextRenderer::MeasureText(const std::wstring& text)
{
    if (m_pDWriteFactory == nullptr || m_pDefaultTextFormat == nullptr)
        return D2D1::SizeF(0.0f, 0.0f);

    ComPtr<IDWriteTextLayout> pLayout;

    // 텍스트 레이아웃 생성
    HRESULT hr = m_pDWriteFactory->CreateTextLayout(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        m_pDefaultTextFormat.Get(),
        10000.0f,
        10000.0f,
        &pLayout
    );

    if (FAILED(hr) || pLayout == nullptr)
        return D2D1::SizeF(0.0f, 0.0f);

    DWRITE_TEXT_METRICS metrics;
    // 레이아웃으로부터 계산된 텍스트 크기 정보 획득
    pLayout->GetMetrics(&metrics);

    return D2D1::SizeF(metrics.width, metrics.height);
}