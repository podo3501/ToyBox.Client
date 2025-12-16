#pragma once
#include "Public/IRenderer.h"
#include "Config/Setting.h"

class Texture;
class CFont;
class RenderTexture;
class TextureResource;
namespace DX { class DeviceResources; }

class TextureRepository : public ITextureLoad, public ITextureController, public ITextureRender
{
public:
    TextureRepository(DX::DeviceResources* deviceRes,
        DescriptorHeap* descriptorHeap,
        ResourceUploadBatch* upload, 
        SpriteBatch* sprite); //SpriteBatch는 여기서 만들어 줄 수 있다.
    ~TextureRepository();

    //ITextureLoad
    virtual bool LoadTexture(const wstring& filename, size_t& outIndex, XMUINT2* outSize, UINT64* outGfxMemOffset) override;
    virtual bool LoadFont(const wstring& filename, size_t& outIndex) override;

    //ITextureController
    virtual void SetTextureRenderer(function<void(size_t index, ITextureRender*)> rendererFn) noexcept override;
    virtual bool CreateRenderTexture(const Rectangle& targetRect, size_t& outIndex, UINT64* outGfxMemOffset) override;
    virtual Rectangle MeasureText(size_t index, const wstring& text, const Vector2& position) override;
    virtual float GetLineSpacing(size_t index) const noexcept override;
    virtual optional<vector<Rectangle>> GetTextureAreaList(size_t index, const UINT32& bgColor) override;
    virtual void ReleaseTexture(size_t idx) noexcept override;
    virtual void ModifyRenderTexturePosition(size_t index, const XMINT2& leftTop) noexcept override;
    virtual bool ModifyRenderTextureSize(size_t index, const XMUINT2& size) override;
    virtual void AddRef(size_t index) noexcept override;

    //ITextureRender
    virtual void Render(size_t index, const RECT& dest, const RECT* source) override;
    virtual void DrawString(size_t index, const wstring& text, const Vector2& pos, const FXMVECTOR& color) const override;

    void DrawRenderTextures();
    void Reset();

private:
    template<typename TexResType>
    bool LoadTextureResource(const wstring& filename, size_t& outIndex, function<void(TextureResource*)> postLoadAction);
    TextureResource* FindTextureResource(const wstring& filename) const noexcept;
    TextureResource* FindTextureResource(size_t index) const noexcept;
    void ReleaseDescriptor(size_t idx) noexcept;
    size_t AllocateDescriptor() noexcept;

    DX::DeviceResources* m_deviceResources;
    ID3D12Device* m_device;
    DescriptorHeap* m_descHeap;
    ResourceUploadBatch* m_upload;
    SpriteBatch* m_sprite;

    function<void(size_t index, ITextureRender*)> m_textureRenderer;
    array<unique_ptr<TextureResource>, MAX_DESC> m_texResources;
    array<int, MAX_DESC> m_refCount{};
    vector<size_t> m_freeDescIndices; //해제되고나서 재활용될 인덱스 모음
    size_t m_nextDescIdx{ 0 };  //새롭게 할당할 인덱스
};