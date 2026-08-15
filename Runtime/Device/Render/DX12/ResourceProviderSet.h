#pragma once
#include "GameClient/Service/Render/IResourceProviderSet.h"
#include "Provider/Font/FontProvider.h"
#include "Provider/Mesh/MeshProvider.h"
#include "Provider/Texture/TextureProvider.h"
#include "Provider/Material/MaterialProvider.h"
#include "Provider/Material/DebugMaterialProvider.h"
#include "Provider/Brush/BrushProvider.h"
#include "Provider/Texture/TextureCubeProvider.h"
#include "Provider/Environment/EnvironmentProvider.h"

struct ShaderRegisterDesc;
class Device;
class DescriptorFactory;
class ResourceFactory;
class TaskScheduler;
class ShaderLibrary;

class ResourceProviderSet : public IResourceProviderSet
{
public:
	~ResourceProviderSet();
	ResourceProviderSet(
		Device& device,
		TaskScheduler& taskScheduler,
		ResourceFactory& resFactory,
		DescriptorFactory& descFactory);

	virtual IResourceProvider* GetFontProvider() override { return &m_fontProvider; }
	virtual IResourceProvider* GetMeshProvider() override { return &m_meshProvider; }
	virtual IResourceProvider* GetMaterialProvider() override { return &m_matProvider; }
	virtual IResourceProvider* GetDebugMaterialProvider() override { return &m_debugMatProvider; }
	virtual IResourceProvider* GetBrushProvider() override { return &m_brushProvider; }
	virtual IResourceProvider* GetEnvironmentProvider() override { return &m_envProvider; }
	
	bool Initialize(ShaderLibrary& shaderLibaray);
	void Update(float gpuMs);

private:
	Device& m_device;

	FontProvider m_fontProvider;
	MeshProvider m_meshProvider;
	TextureProvider m_texProvider;
	MaterialProvider m_matProvider;
	DebugMaterialProvider m_debugMatProvider;
	BrushProvider m_brushProvider;
	TextureCubeProvider m_cubeProvider;
	EnvironmentProvider m_envProvider;

	float m_avgGpuMs{ 0.0f };
};
