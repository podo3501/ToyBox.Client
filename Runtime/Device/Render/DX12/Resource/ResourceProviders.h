#pragma once
#include "Texture/TextureProvider.h"
#include "Mesh/MeshProvider.h"
#include "Material/MaterialProvider.h"
#include "Shader/ShaderProvider.h"

struct ShaderRegisterDesc;
class Device;
class DescriptorFactory;
class ResourceFactory;
class TaskScheduler;

class ResourceProviders
{
public:
	~ResourceProviders();
	ResourceProviders(
		Device& device,
		DescriptorFactory& descFactory,
		ResourceFactory& resFactory,
		TaskScheduler& taskScheduler);

	bool Initialize(const std::vector<ShaderRegisterDesc>& shaders);
	void Update(float gpuMs);

	ShaderProvider& GetShaderProvider() { return m_shaderProvider; }
	MeshProvider& GetMeshProvider() { return m_meshProvider; }
	MaterialProvider& GetMaterialProvider() { return m_matProvider; }

private:
	Device& m_device;

	ShaderProvider m_shaderProvider;
	TextureProvider m_texProvider;
	MeshProvider m_meshProvider;
	MaterialProvider m_matProvider;
};
