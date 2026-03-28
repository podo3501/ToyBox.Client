#include "pch.h"
#include "DX12RenderBackend.h"

DX12RenderBackend::~DX12RenderBackend() = default;
DX12RenderBackend::DX12RenderBackend() = default;

bool DX12RenderBackend::Initialize()
{
	return true;
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	auto backend = make_unique<DX12RenderBackend>();
	if (!backend->Initialize()) return nullptr;

	return backend;
}
