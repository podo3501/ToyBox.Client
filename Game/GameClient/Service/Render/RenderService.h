#pragma once
#include "Repository/RenderRepository.h"
#include "Core/Math/Matrix.h"
#include "RenderState.h"

struct IRenderBackend;
struct Rect;
struct Size;
struct DrawCommand;
struct DrawMeshCommand;
class RenderRepository;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	void SetRasterState(const RasterState& rasterState);
	void DrawUI(TextureHandle th, const Rect& dest, const Rect* source);
	void DrawMesh(MeshHandle hM, const Core::Math::Matrix& world);
	//void DrawMesh(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderRepository* GetRepository();

private:
	RenderService(unique_ptr<IRenderBackend> backend);

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<RenderRepository> m_repository;
};