#pragma once
#include "GameClient/Service/Render/IResourceProvider.h"
#include "Provider/Texture/TextureProvider.h"
#include "Provider/Mesh/MeshProvider.h"
#include "Provider/Material/MaterialProvider.h"

struct ShaderRegisterDesc;
class Device;
class DescriptorFactory;
class ResourceFactory;
class TaskScheduler;
class ShaderLibrary;

class ResourceProvider : public IResourceProvider
{
public:
	~ResourceProvider();
	ResourceProvider(
		Device& device,
		TaskScheduler& taskScheduler,
		ResourceFactory& resFactory,
		DescriptorFactory& descFactory);

	virtual IMeshProvider* GetMeshProvider() override { return &m_meshProvider; }
	virtual IMaterialProvider* GetMaterialProvider() override { return &m_matProvider; }
	
	bool Initialize(ShaderLibrary& shaderLibaray);
	void Update(float gpuMs);

private:
	Device& m_device;

	TextureProvider m_texProvider;
	MeshProvider m_meshProvider;
	MaterialProvider m_matProvider;

	float m_avgGpuMs{ 0.0f };
};
