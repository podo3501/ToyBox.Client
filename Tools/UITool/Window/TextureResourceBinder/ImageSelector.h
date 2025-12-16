#pragma once

enum class TextureSlice;
class TextureResBinderWindow;
class TexResCommandHistory;
struct TextureSourceInfo;
class PatchTextureStd1;
class RenameNotifier;
class ImageSelector
{
public:
	~ImageSelector();
	ImageSelector(TextureResBinderWindow* textureWindow);

	void Update();
	void Render();
	void SetTexture(PatchTextureStd1* pTex1) noexcept;
	void SetCommandHistory(TexResCommandHistory* cmdHistory) noexcept { m_cmdHistory = cmdHistory; }
	bool DeselectArea() noexcept;

private:
	void CheckSourcePartition() noexcept;
	void CheckSelectArea() noexcept;
	void SelectArea() noexcept;	
	bool RemoveArea() noexcept;
	optional<Rectangle> FindAreaFromMousePos(const XMINT2& pos) const noexcept;
	void SelectImagePart();
	void EditSelectArea();
	void RenderHoveredArea() const;
	void RenderSelectedArea() const;
	void RenderLabeledAreas() const;
	

	PatchTextureStd1* m_sourceTexture;
	TexResCommandHistory* m_cmdHistory;
	TextureResBinderWindow* m_textureWindow;
	unique_ptr<RenameNotifier> m_renameNotifier;
	TextureSlice m_selectImagePart;
	vector<Rectangle> m_areaList; //자동으로 찾아놓은 구역들
	vector<Rectangle> m_hoveredAreas{}; //마우스를 갖대댔을때의 구역들
	unique_ptr<TextureSourceInfo> m_selectedArea;
};