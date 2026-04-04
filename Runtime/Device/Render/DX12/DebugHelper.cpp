#include "pch.h"
#include "DebugHelper.h"
#include "DebugOptions.h"
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>

void DebugHelper::EnableDebugLayer(const DebugOptions& opt)
{
#if defined(_DEBUG)
    if (!opt.enableDebugLayer)
        return;

    ComPtr<ID3D12Debug> debugController;
    if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        return;

    debugController->EnableDebugLayer();

    if (opt.enableGpuValidation)
    {
        ComPtr<ID3D12Debug1> debugController1;
        if (SUCCEEDED(debugController.As(&debugController1)))
        {
            debugController1->SetEnableGPUBasedValidation(TRUE);
        }
    }
#endif
}

void DebugHelper::SetupInfoQueue(ID3D12Device* device, const DebugOptions& opt)
{
#if defined(_DEBUG)
    if (!device)
        return;

    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
    {
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

        if (opt.breakOnWarning)
        {
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
        }

        // 선택: noisy 메시지 필터링
        D3D12_MESSAGE_ID denyIds[] =
        {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
        };

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;

        infoQueue->AddStorageFilterEntries(&filter);
    }
#endif
}