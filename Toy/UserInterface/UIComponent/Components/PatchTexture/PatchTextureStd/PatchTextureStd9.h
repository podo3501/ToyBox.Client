#pragma once
#include "PatchTextureStd.h"

struct TextureSourceInfo;
class PatchTextureStd3;

class PatchTextureStd9 : public PatchTextureStd
{
public:
	~PatchTextureStd9();
	PatchTextureStd9() noexcept;

	static ComponentID GetTypeStatic() { return ComponentID::PatchTextureStd9; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }

	bool Setup(const UILayout& layout, const string& bindKey);
	inline bool Setup(const string& bindKey) { return Setup({}, bindKey); }

protected:
	PatchTextureStd9(const PatchTextureStd9& o) noexcept;
	virtual unique_ptr<UIComponent> CreateClone() const override;
};