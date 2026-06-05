#pragma once
#include "SingletonBase.h"
#include <d2d1_3.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>

class ResourceManager : public SingletonBase<ResourceManager>
{
    friend class SingletonBase<ResourceManager>;

public:
    // WIC 팩토리 생성 및 컨텍스트 연결
    bool Initialize(ID2D1DeviceContext7* pContext);
    void Release();

    // 외부 이미지 파일을 읽어 비트맵으로 변환 및 보관
    bool LoadWICBitmap(const std::wstring& key, const std::wstring& filePath);

    // 보관된 비트맵 반환 (O(1) 탐색)
    ID2D1Bitmap* GetBitmap(const std::wstring& key) const;

private:
    ResourceManager() = default;
    ~ResourceManager() { Release(); }

    ID2D1DeviceContext7* m_pContext = nullptr; // 렌더러에서 참조용으로 받음
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_pWICFactory;

    // 비트맵 저장소 (Key: 고유 식별 문자열, Value: 비트맵 객체)
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_bitmapMap;
};