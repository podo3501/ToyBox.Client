#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Definition/Material/UIMaterialDesc.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Geometry2D.h"

struct ITextureResource;

class UIMaterialResource : public MaterialResource
{
public:
	~UIMaterialResource();
	UIMaterialResource() = delete;
	UIMaterialResource(const MaterialDesc& desc);

	virtual std::vector<BuiltinTextureBinding> GetBuiltinTextureBindings() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	Core::Vector4 CalcUVTransform(const Rect* source);

private:
	UIMaterialDesc m_desc;
	Core::Vector4 m_uvTransform{ 0.0f, 0.0f, 1.0f, 1.0f };
};