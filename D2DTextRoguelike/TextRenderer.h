// TextRoguelike 용 별도 엔진 파일
#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

// 정렬 방식 지정용 열거형 추가
enum class TextAlign
{
    Left,
    Center,
    Right
};

// 세로 정렬
enum class VerticalAlign
{
    Top,
    Center,
    Bottom
};

class TextRenderer
{
public:
    TextRenderer() = default;
    ~TextRenderer();

    bool Initialize(ID2D1DeviceContext7* pContext);
    void Release();

    // 텍스트 포맷 생성 및 기본 폰트 설정
    bool CreateTextFormat(const std::wstring& formatKey, const std::wstring& fontName, float fontSize, IDWriteFontCollection* pCollection);

    // 문자열 출력 (기본값은 왼쪽 정렬)
    void DrawText(const std::wstring& text, float x, float y, float width, float height,
        const D2D1::ColorF& color,
        TextAlign align = TextAlign::Left,
        VerticalAlign vAlign = VerticalAlign::Top,
        const std::wstring& formatKey = L"Default");

    // 문자열이 차지하는 영역 계산 (UI_Menu 크기 자동화 등에 사용)
    D2D1_SIZE_F MeasureText(const std::wstring& text, const std::wstring& formatKey);


private:
    ID2D1DeviceContext7* m_pContext = nullptr; // 렌더러에서 전달받아 참조만 함 (메모리 해제 X)

    ComPtr<IDWriteFactory5> m_pDWriteFactory5;
    ComPtr<IDWriteFontCollection1> m_pCustomFontCollection;

    std::unordered_map<std::wstring, ComPtr<IDWriteTextFormat>> m_textFormats;
    ComPtr<ID2D1SolidColorBrush> m_pTextBrush;
};