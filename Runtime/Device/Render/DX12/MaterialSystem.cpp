#include "pch.h"
#include "MaterialSystem.h"
#include "MaterialResource.h"

MaterialSystem::~MaterialSystem() = default;
MaterialSystem::MaterialSystem(ID3D12Device* device)
{}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource(std::shared_ptr<ITextureResource> texRes)
{
	return make_shared<MaterialResource>(texRes);
}