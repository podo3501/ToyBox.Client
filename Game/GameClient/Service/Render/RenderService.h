#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "RenderMetrics.h"
#include "RenderRepository.h"
#include "SceneRenderer.h"
#include "Definition/ShaderDesc.h"
#include "Repository/Container/RepositoryContainer.h"

struct IRenderBackend;
struct FrameData;
struct IAssetAsyncLoader;
struct RegistryShaderEntry;
class IResourceRepository;
class MaterialRepo;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(
		unique_ptr<IRenderBackend> backend,
		IAssetAsyncLoader* asyncLoader) noexcept;

	bool Initialize(HWND hwnd, const Size& screenSize);
	ShaderID RegisterShader(const Core::ResourceID& resID, ShaderType type);
	void SetFrameData(const FrameData& frameData);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderRepository& GetRepository() { return *m_repository; }
	SceneRenderer& GetRenderer() { return *m_renderer; }
	RenderMetrics GetRenderMetrics();

private:
	RenderService(unique_ptr<IRenderBackend> backend, IAssetAsyncLoader* asyncLoader);
	std::vector<RegistryShaderEntry> LoadRegistryShaderEntries();
	std::vector<RegistryShaderDesc> SetupRegistryShaders();

	unique_ptr<IRenderBackend> m_backend;
	IAssetAsyncLoader* m_asyncLoader{ nullptr };
	std::unordered_map<Core::ResourceID, ShaderID> m_shaderCache;

	RepositoryContainer m_repositories;
	unique_ptr<RenderRepository> m_repository;
	unique_ptr<SceneRenderer> m_renderer;
};