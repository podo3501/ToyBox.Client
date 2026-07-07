#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "RenderMetrics.h"
#include "RenderRepository.h"
#include "SceneRenderer.h"
#include "Desc/ShaderStageBuilder.h"

struct IRenderBackend;
struct FrameData;
struct IAssetAsyncLoader;
struct RegistryShaderEntry;
class FontRepository;
class MeshRepository;
class MaterialRepository;

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

	RenderRepository* GetRepository() { return m_repository.get(); }
	SceneRenderer* GetRenderer() { return m_renderer.get(); }
	RenderMetrics GetRenderMetrics();

private:
	RenderService(unique_ptr<IRenderBackend> backend, IAssetAsyncLoader* asyncLoader);
	std::vector<RegistryShaderEntry> LoadRegistryShaderEntries();
	std::vector<RegistryShaderDesc> SetupRegistryShaders();

	unique_ptr<IRenderBackend> m_backend;
	IAssetAsyncLoader* m_asyncLoader{ nullptr };
	std::unordered_map<Core::ResourceID, ShaderID> m_shaderCache;

	unique_ptr<FontRepository> m_fontRepository;
	unique_ptr<MeshRepository> m_meshRepository;
	unique_ptr<MaterialRepository> m_matRepository;

	unique_ptr<RenderRepository> m_repository;
	unique_ptr<SceneRenderer> m_renderer;
};