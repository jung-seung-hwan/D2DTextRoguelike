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
    ComPtr<IDWriteFactory> pBaseFactory;
    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(pBaseFactory.GetAddressOf())
    );

    if (FAILED(hr)) return false;

    hr = pBaseFactory.As(&m_pDWriteFactory5);
    if (FAILED(hr)) return false;

    ComPtr<IDWriteFontSetBuilder1> pFontSetBuilder;
    hr = m_pDWriteFactory5->CreateFontSetBuilder(&pFontSetBuilder);
    if (SUCCEEDED(hr))
    {
        ComPtr<IDWriteFontFile> pFontFile;
        // 실행 파일 기준의 상대 경로
        std::wstring fontPath = L"Resource/neodgm.ttf";

        hr = m_pDWriteFactory5->CreateFontFileReference(fontPath.c_str(), nullptr, &pFontFile);
        if (SUCCEEDED(hr))
        {
            pFontSetBuilder->AddFontFile(pFontFile.Get());

            ComPtr<IDWriteFontSet> pFontSet;
            pFontSetBuilder->CreateFontSet(&pFontSet);

            // 폰트 집합으로부터 독립적인 컬렉션 생성
            m_pDWriteFactory5->CreateFontCollectionFromFontSet(pFontSet.Get(), &m_pCustomFontCollection);
        }
    }

    // 기본 폰트 설정
    if (!CreateTextFormat(L"Default", L"Neo둥근모", 15.0f, m_pCustomFontCollection.Get()))
        return false;

    // UI 화면용 폰트 생성 
    if (!CreateTextFormat(L"Title", L"Neo둥근모", 30.0f, m_pCustomFontCollection.Get()))
        return false;

    return true;
}

void TextRenderer::Release()
{
    // 스마트포인터인 Comptr 사용했지만 리소스 해제와 생명주기 관리를 위해 별도로 Release 함수 사용
    // ex) TextRenderer 객체는 살리고 내부 자원만 초기화하는 경우
    // 향후 해상도 변경, 언어 변경, 폰트 교체에 사용하기 위해 일단 작성
    m_pTextBrush.Reset();
    m_textFormats.clear();
    m_pDWriteFactory5.Reset();
    m_pContext = nullptr;
}

bool TextRenderer::CreateTextFormat(const std::wstring& formatKey, const std::wstring& fontName, float fontSize, IDWriteFontCollection* pCollection) 
{
    if (m_pDWriteFactory5 == nullptr) return false;

    ComPtr<IDWriteTextFormat> pTextFormat;

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
    HRESULT hr = m_pDWriteFactory5->CreateTextFormat(
        fontName.c_str(),
        pCollection,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"ko-kr",
        &pTextFormat
    );


    // 문단 정렬 (세로)
    pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    // 자동 줄바꿈
    pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

    // 생성된 서식을 맵에 등록
    m_textFormats[formatKey] = pTextFormat;

    return true;
}


/*
* 매개변수 정보
    text : 화면에 출력할 문자열
    x : 텍스트 영역의 시작 X 좌표
    y : 텍스트 영역의 시작 Y 좌표
    width : 텍스트가 출력될 영역의 너비
    height : 텍스트가 출력될 영역의 높이
    color : 텍스트 색상
    align : 가로 정렬 방식
    verticalalign : 세로 정렬 방식
*/
void TextRenderer::DrawText(const std::wstring& text, float x, float y, float width, float height,
    const D2D1::ColorF& color, TextAlign align, VerticalAlign vAlign,
    const std::wstring& formatKey)
{
    if (m_pContext == nullptr) return;

    // 맵에서 폰트 서식 찾기
    auto it = m_textFormats.find(formatKey);
    if (it == m_textFormats.end()) return; // 등록되지 않은 폰트면 렌더링 중지

    IDWriteTextFormat* pCurrentFormat = it->second.Get();

    // 텍스트를 그릴 색상 브러시 생성
    if (m_pTextBrush == nullptr)
    {
        HRESULT hr = m_pContext->CreateSolidColorBrush(color, &m_pTextBrush);
        if (FAILED(hr))
            return;
    }
    else
    {
        // 기존 브러시가 있으면 새로 브러쉬 만들지 않고 색상만 변경
        m_pTextBrush->SetColor(color);
    }

    // 호출 시 전달받은 정렬 방식에 따라 서식의 속성을 동적으로 변경
    switch (align)
    {
    case TextAlign::Left:   pCurrentFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); break;
    case TextAlign::Center: pCurrentFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); break;
    case TextAlign::Right:  pCurrentFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING); break;
    }

    switch (vAlign)
    {
    case VerticalAlign::Top:    pCurrentFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); break;
    case VerticalAlign::Center: pCurrentFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); break;
    case VerticalAlign::Bottom: pCurrentFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR); break;
    }

    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + width, y + height);

    m_pContext->DrawTextW(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        pCurrentFormat,
        layoutRect,
        m_pTextBrush.Get(),
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL
    );
}

// 현재 폰트가 공간이 얼마나 필요한지 계산 -> 실제 픽셀 크기 계산
D2D1_SIZE_F TextRenderer::MeasureText(const std::wstring& text, const std::wstring& formatKey)
{
    if (m_pDWriteFactory5 == nullptr)
        return D2D1::SizeF(0.0f, 0.0f);

    // 맵에서 전달받은 키에 해당하는 폰트 서식 검색
    auto it = m_textFormats.find(formatKey);
    if (it == m_textFormats.end())
        return D2D1::SizeF(0.0f, 0.0f); 

    IDWriteTextFormat* pCurrentFormat = it->second.Get();

    ComPtr<IDWriteTextLayout> pLayout;

    // 텍스트 레이아웃 생성 시 검색된 서식 포인터(pCurrentFormat) 적용
    HRESULT hr = m_pDWriteFactory5->CreateTextLayout(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        pCurrentFormat,
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