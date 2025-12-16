#include "pch.h"
#include "DxLeakCheck.h"

#if defined(DEBUG) | defined(_DEBUG)
void CheckDirectxLeaks()
{
	HMODULE dxgidebugdll = GetModuleHandleW(L"dxgidebug.dll");
	if (dxgidebugdll == NULL) return;

	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));
	if (GetDebugInterface == nullptr) return;

	IDXGIDebug* debug;
	GetDebugInterface(IID_PPV_ARGS(&debug));

	OutputDebugStringW(L"---------------Starting Live Direct3D Object Dump:----------------\r\n");
	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	OutputDebugStringW(L"---------------Completed Live Direct3D Object Dump----------------\r\n");

	debug->Release();
}
#endif