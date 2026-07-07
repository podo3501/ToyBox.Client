#pragma once
#include "GameClient/Service/Render/IResourceProvider.h"
#include "Core/FreeTypeLibrary.h"
#include "Provider/Font/FontProvider.h"
#include "Provider/Mesh/MeshProvider.h"
#include "Provider/Texture/TextureProvider.h"
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

	virtual IFontProvider* GetFontProvider() override { return nullptr; }
	virtual IMeshProvider* GetMeshProvider() override { return &m_meshProvider; }
	virtual IMaterialProvider* GetMaterialProvider() override { return &m_matProvider; }
	
	bool Initialize(ShaderLibrary& shaderLibaray);
	void Update(float gpuMs);

private:
	Device& m_device;
	FreeTypeLibrary m_ftLibrary;

	FontProvider m_fontProvider;
	MeshProvider m_meshProvider;
	TextureProvider m_texProvider;
	MaterialProvider m_matProvider;

	float m_avgGpuMs{ 0.0f };
};
