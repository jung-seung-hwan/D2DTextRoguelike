#include "pch.h"
#include "D2DApp.h"


int main()
{
	// 해제되지 않는 메모리 검사
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM 초기화 -> windows 그래픽 관련 기능 사용하기 위해 초기화 진행
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
		return -1;

	D2DApp testApp;

	if (!testApp.Initialize())
	{
		std::cerr << "Failed to initialize sample code." << std::endl;
		return -1;
	}

	testApp.Run();

	testApp.Finalize();

	CoUninitialize();


	return 0;
}
