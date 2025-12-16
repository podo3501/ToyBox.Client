#pragma once
#include "../UIComponent.h"

namespace DX { class StepTimer; }

class RenderTexture : public UIComponent
{
public:
	~RenderTexture();
	RenderTexture();

	static ComponentID GetTypeStatic() { return ComponentID::RenderTexture; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual bool operator==(const UIComponent& rhs) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;

	bool Setup(const UILayout& layout, unique_ptr<UIComponent> component) noexcept;
	bool Setup(unique_ptr<UIComponent> component) noexcept;
	inline UINT64 GetGraphicMemoryOffset() const noexcept { return m_gfxOffset; }
	inline UIComponent* GetRenderedComponent() const noexcept { return m_component; }

protected:
	RenderTexture(const RenderTexture& other);
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override;
	virtual bool ChangeSize(const XMUINT2& size, bool isForce) noexcept override;
	virtual void Render(ITextureRender* render) const override;

private:
	bool CreateComponentTexture();
	void Release() noexcept;
	void ReloadDatas() noexcept;

	UIComponent* m_component;
	optional<size_t> m_index;
	UINT64 m_gfxOffset{};
};
