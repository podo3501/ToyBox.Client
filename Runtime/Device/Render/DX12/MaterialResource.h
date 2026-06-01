#pragma once
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include <memory>

struct ITextureResource;
struct MaterialResource : public IMaterialResource //?!? backend용 material resource 인터페이스. 나중에 인터페이스가 아니라 실제 데이터를 갖는 방식으로 수정.
{
	virtual void MarkReady() noexcept = 0;
	virtual bool IsTextureReady() const noexcept = 0;
	virtual MaterialType GetType() const noexcept = 0;
	virtual void SetMaterialDesc(const MaterialDesc& desc) noexcept = 0;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept = 0;
	virtual void SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes) noexcept = 0;
	virtual std::vector<std::shared_ptr<ITextureResource>> GetTextures() const noexcept = 0;
};