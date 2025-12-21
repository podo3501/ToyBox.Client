#include "pch.h"
#include "IRenderer.h"
#include "Toy/GameLoop.h"
#include "Shared/Framework/Initializer/Application.h"
#include "Shared/Utils/PathUtils.h"
#include "Core/Utils/DxLeakCheck.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//괄호로 감싼 이유는 MainLoop의 스마트 포인터 소멸자를 호출해 주기 위해서이며,
	//DirectX 관련 리소스가 잘 소멸됐는지 ReportLiveObjects함수로 확인하기 때문이다.
	int nResult = { 0 };
	{
		RECT windowRect = { 0, 0, 800, 600 };
		auto root = FindRootByMarker(L"root.mark");
		auto resourcePath = root + L"Resources/";
		auto appLoop = CreateAppLoop<GameLoop>(hInstance, nShowCmd, windowRect, resourcePath, false);
		if (!appLoop)
			return 1;

		nResult = appLoop->Run();
		appLoop.reset();
	}

#if defined(DEBUG) | defined(_DEBUG)
	CheckDirectxLeaks();
#endif

	return nResult;
}