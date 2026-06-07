#include "pch.h"
#include "RenderContext.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Texture/TextureRepository.h"
#include "Repository/Mesh/MeshRepository.h"
#include "Service/Asset/Assets/MeshAsset.h"
#include "Desc/MeshDesc.h"
#include "Resource/IMeshResource.h"
#include "IRenderBackend.h"

namespace cm = Core::Math;

RenderContext::~RenderContext() { m_backend->WaitIdle(); } //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
RenderContext::RenderContext(IRenderBackend* backend, AssetPipelineT* assetPipeline) :
	m_backend{ backend },
	m_texRepository{ make_unique<TextureRepository>(m_backend->GetTextureSystem(), assetPipeline) },
	m_meshRepository{ make_unique<MeshRepository>(m_backend->GetMeshSystem(), assetPipeline) },
	m_matRepository{ make_unique<MaterialRepository>(m_backend->GetMaterialSystem(), assetPipeline) }
{}

bool RenderContext::Initialize()
{
	MeshDesc meshDesc{ Core::ResourceID::MakeBuiltin("ui_quad") };
	m_uiQuad = m_meshRepository->GetOrCreate(meshDesc, CreateUIQuad());
	return true;
}

MeshHandle RenderContext::LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset)
{
	return m_meshRepository->GetOrCreate(desc, asset);
}

bool RenderContext::ReleaseMesh(MeshHandle mh)
{
	return m_meshRepository->Release(mh);
}

TextureHandle RenderContext::LoadTexture(const TextureDesc& desc)
{
	return m_texRepository->GetOrCreate(desc);
}

bool RenderContext::ReleaseTexture(TextureHandle th)
{
	return m_texRepository->Release(th);
}

MaterialHandle RenderContext::LoadMaterial(const MaterialDesc& desc)
{
	return m_matRepository->GetOrCreate(desc);
}

bool RenderContext::ReleaseMaterial(MaterialHandle mh)
{
	return m_matRepository->Release(mh);
}

void RenderContext::DrawSurface(MeshHandle hM, MaterialHandle hMtl, const cm::Matrix& world)
{
	auto mesh = m_meshRepository->Get(hM);
	if (!mesh || mesh->state != LoadState::Ready)
		return;

	std::shared_ptr<IMaterialResource> matRes;
	if (hMtl)
	{
		auto material = m_matRepository->Get(hMtl);
		if (!material || material->state != LoadState::Ready)
			return;

		matRes = material->matRes;
	}
	else
		matRes = nullptr;

	m_backend->DrawSurface(mesh->meshRes, matRes, world);
}

void RenderContext::DrawUI(MaterialHandle mh, const Rect& dest, const Rect* source)
{
	auto mesh = m_meshRepository->Get(m_uiQuad);
	if (!mesh || mesh->state != LoadState::Ready)
		return;

	std::shared_ptr<IMaterialResource> matRes;
	if (mh)
	{
		auto material = m_matRepository->Get(mh);
		if (!material || material->state != LoadState::Ready)
			return;

		matRes = material->matRes;
	}
	else
		matRes = nullptr;

	float width = static_cast<float>(dest.width);
	float height = static_cast<float>(dest.height);

	float centerX = dest.x + width * 0.5f;
	float centerY = dest.y + height * 0.5f;

	cm::Matrix translation = cm::Matrix::Translation(centerX, centerY, 0.0f);
	cm::Matrix scale = cm::Matrix::Scale(width, height, 1.0f);
	cm::Matrix world = scale * translation;
	
	if (source) // 텍스쳐의 부분을 가지고 올때 사용함. 
	{
		// float u0 = source->x / textureWidth;
		// float v0 = source->y / textureHeight;
		// float u1 = (source->x + source->w) / textureWidth;
		// float v1 = (source->y + source->h) / textureHeight;
		//
		// Vector2 uvScale  = { u1 - u0, v1 - v0 };
		// Vector2 uvOffset = { u0, v0 };
		//
		// matRes->SetUVTransform(uvScale, uvOffset);
	}

	m_backend->DrawUI(mesh->meshRes, matRes, world);
}

void RenderContext::Update()
{
	m_meshRepository->Update();
	m_texRepository->Update();
	m_matRepository->Update();
}

void RenderContext::ReleaseAll()
{
	m_meshRepository->ReleaseAll();
	m_texRepository->ReleaseAll();
	m_matRepository->ReleaseAll();
}

std::shared_ptr<MeshAsset> RenderContext::CreateUIQuad()
{
	auto asset = std::make_shared<MeshAsset>();
	asset->format = VertexFormat::UI;

	std::vector<UIVertex> vertices =
	{
		{ -0.5f, -0.5f, 0, 1,1,1,1, 0,1 },
		{ -0.5f,  0.5f, 0, 1,1,1,1, 0,0 },
		{  0.5f, -0.5f, 0, 1,1,1,1, 1,1 },
		{  0.5f,  0.5f, 0, 1,1,1,1, 1,0 },
	};

	std::vector<uint32_t> indices =
	{
		0,1,2,
		2,1,3
	};

	asset->SetVertices(vertices);
	asset->indices = std::move(indices);

	return asset;
}