#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"

class DX12RenderBackend : public IRenderBackend
{
public:
	~DX12RenderBackend();
	DX12RenderBackend();
	bool Initialize();

private:
};