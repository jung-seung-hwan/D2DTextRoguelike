#include "pch.h"
#include "ResourceManager.h"

bool ResourceManager::Initialize(ID2D1DeviceContext7* pContext)
{
    if (pContext == nullptr) return false;
    m_pContext = pContext;

    // WIC 팩토리 객체 생성
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pWICFactory)
    );

    return SUCCEEDED(hr);
}

bool ResourceManager::LoadWICBitmap(const std::wstring& key, const std::wstring& filePath)
{
    if (m_pContext == nullptr || m_pWICFactory == nullptr) return false;

    // 이미 등록된 키라면 중복 로드 방지
    if (m_bitmapMap.find(key) != m_bitmapMap.end()) return true;

    HRESULT hr = S_OK;

    // 디코더 생성 (파일 열기)
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
    hr = m_pWICFactory->CreateDecoderFromFilename(
        filePath.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );
    if (FAILED(hr)) return false;

    // 첫 번째 프레임 가져오기
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
    hr = pDecoder->GetFrame(0, &pFrame);
    if (FAILED(hr)) return false;

    // 포맷 변환기 생성 (Direct2D 규격에 맞추기 위해)
    Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter;
    hr = m_pWICFactory->CreateFormatConverter(&pConverter);
    if (FAILED(hr)) return false;

    // 투명도(Alpha)를 지원하는 32bppPBGRA 포맷으로 초기화
    hr = pConverter->Initialize(
        pFrame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
    );
    if (FAILED(hr)) return false;

    // 변환된 WIC 데이터를 기반으로 Direct2D 하드웨어 비트맵 생성
    Microsoft::WRL::ComPtr<ID2D1Bitmap> pD2DBitmap;
    hr = m_pContext->CreateBitmapFromWicBitmap(
        pConverter.Get(),
        nullptr,
        &pD2DBitmap
    );
    if (FAILED(hr)) return false;

    // 생성 완료된 비트맵을 해시맵에 캐싱
    m_bitmapMap[key] = pD2DBitmap;

    return true;
}

ID2D1Bitmap* ResourceManager::GetBitmap(const std::wstring& key) const
{
    auto it = m_bitmapMap.find(key);
    if (it != m_bitmapMap.end())
    {
        return it->second.Get();
    }
    return nullptr;
}

void ResourceManager::Release()
{
    m_bitmapMap.clear();
    m_pWICFactory.Reset();
    m_pContext = nullptr;
}