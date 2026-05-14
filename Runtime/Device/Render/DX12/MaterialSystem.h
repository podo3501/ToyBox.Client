#pragma once
#include "GameClient/Service/Render/Repository/IMaterialSystem.h"

struct ID3D12Device;

class MaterialSystem : public IMaterialSystem
{
public:
	~MaterialSystem();
	MaterialSystem(ID3D12Device* device);
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(std::shared_ptr<ITextureResource> texRes) override;
};
