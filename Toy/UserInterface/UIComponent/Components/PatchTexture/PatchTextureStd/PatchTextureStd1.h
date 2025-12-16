#pragma once
#include "PatchTextureStd.h"
#include "../PatchTextureCoord.h"

struct IRenderer;
struct TextureSourceInfo;
namespace DX { class StepTimer; }

class PatchTextureStd1 : public PatchTextureStd
{
public:
	~PatchTextureStd1();
	PatchTextureStd1();

	static ComponentID GetTypeStatic() { return ComponentID::PatchTextureStd1; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual bool operator==(const UIComponent& rhs) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;
	//PatchTexture
	virtual bool FitToTextureSource() noexcept override { return m_coord.FitToTextureSource(); }
	virtual UIComponent* GetCenterComponent() noexcept override { return this; }
	virtual const string& GetBindKey() const noexcept override { return m_bindKey; }

	optional<vector<Rectangle>> GetTextureAreaList();
	bool Setup(const UILayout& layout, const string& bindKey, size_t sourceIndex = 0u) noexcept;
	bool Setup(const TextureSourceInfo& sourceInfo, ITextureController* texController) noexcept; //텍스쳐를 단독으로 사용할때. 텍스쳐 png를 읽어들일때 사용한다.
	inline  bool Setup(const string& bindKey, size_t sourceIndex = 0u) noexcept { return Setup({}, bindKey, sourceIndex); }

	inline const Rectangle& GetSource() const noexcept { return m_coord.GetSource(); }
	inline wstring GetFilename() const noexcept { return m_filename; }
	inline UINT64 GetGraphicMemoryOffset() const noexcept { return m_gfxOffset; }

protected:
	PatchTextureStd1(const PatchTextureStd1& other);
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override; //Binder가 로딩을 다 하고 여기서 값만 연결한다.
	virtual void Render(ITextureRender* render) const override { m_coord.Render(render); }
	virtual bool ResizeAndAdjustPos(const XMUINT2& size) noexcept override { return UIComponent::ResizeAndAdjustPos(size); }
	virtual bool ChangeBindKeyWithIndex(const string& key, const TextureSourceInfo& sourceInfo, size_t sourceIndex) noexcept override;

private:
	bool SetSourceInfo(const TextureSourceInfo& sourceInfo) noexcept;

	string m_bindKey;
	size_t m_sourceIndex{ 0 }; //1, 3, 9 Patch 일때 몇번째 인지 기록하는 인덱스. 1 Patch 일때에는 0값.

	ITextureController* m_texController{ nullptr };
	wstring m_filename;
	UINT64 m_gfxOffset{}; //툴에서 Imgui window 만들때 사용
	PatchTextureCoord m_coord;
};