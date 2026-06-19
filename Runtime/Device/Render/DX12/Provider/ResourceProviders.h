#pragma once
#include "Texture/TextureProvider.h"
#include "Mesh/MeshProvider.h"
#include "Material/MaterialProvider.h"

struct ShaderRegisterDesc;
class Device;
class DescriptorFactory;
class ResourceFactory;
class TaskScheduler;
class ShaderLibrary;

class ResourceProviders
{
public:
	~ResourceProviders();
	ResourceProviders(
		Device& device,
		DescriptorFactory& descFactory,
		ResourceFactory& resFactory,
		TaskScheduler& taskScheduler);
	
	bool Initialize(ShaderLibrary& shaderLibaray);
	void Update(float gpuMs);

	MeshProvider& GetMeshProvider() { return m_meshProvider; }
	MaterialProvider& GetMaterialProvider() { return m_matProvider; }

private:
	Device& m_device;

	TextureProvider m_texProvider;
	MeshProvider m_meshProvider;
	MaterialProvider m_matProvider;
};
