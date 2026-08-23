#pragma once
#include "GameClient/Service/Render/ProviderType.h"
#include "Provider/PendingLoadQueue.h"
#include "Provider/PendingReleaseQueue.h"
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
class IUpdatableProvider;

class ResourceProviderSet
{
public:
	~ResourceProviderSet();
	ResourceProviderSet(
		Device& device,
		TaskScheduler& taskScheduler,
		ResourceFactory& resFactory,
		DescriptorFactory& descFactory);

	IResourceProvider* GetProvider(ProviderType type)
	{
		return m_providers[static_cast<size_t>(type)];
	}
	
	bool Initialize(ShaderLibrary& shaderLibaray);
	void Update(float gpuMs);

private:
	Device& m_device;

	PendingLoadQueue m_pendingLoad;
	PendingReleaseQueue m_pendingRelease;
	FontProvider m_fontProvider;
	MeshProvider m_meshProvider;
	TextureProvider m_texProvider;
	MaterialProvider m_matProvider;
	DebugMaterialProvider m_debugMatProvider;
	BrushProvider m_brushProvider;
	TextureCubeProvider m_cubeProvider;
	EnvironmentProvider m_envProvider;

	std::array<IResourceProvider*, Core::EnumSize<ProviderType>> m_providers;
	std::vector<IUpdatableProvider*> m_updatables;

	float m_avgGpuMs{ 0.0f };
};
