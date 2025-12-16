#include "pch.h"
#include "TextureRepository.h"
#include "Texture.h"
#include "TextureRenderTarget.h"
#include "Font.h"
#include "External/DeviceResources.h"
#include "Utils/Common.h"

TextureRepository::~TextureRepository()
{
    Assert(ranges::all_of(m_texResources, [](const auto& ptr) { return ptr == nullptr; })); //잘 지워졌는지 확인
}

TextureRepository::TextureRepository(DX::DeviceResources* deviceRes, DescriptorHeap* descriptorHeap, ResourceUploadBatch* upload, SpriteBatch* sprite) :
    m_deviceResources{ deviceRes },
    m_device{ deviceRes->GetD3DDevice() },
    m_descHeap{ descriptorHeap },
    m_upload{ upload }, 
    m_sprite{ sprite }
{}

void TextureRepository::AddRef(size_t index) noexcept
{
    m_refCount[index]++;
}

template<typename TexResType>
bool TextureRepository::LoadTextureResource(const wstring& filename, size_t& outIndex,
    function<void(TextureResource*)> postLoadAction)
{
    if (auto find = FindTextureResource(filename); find)
    {
        outIndex = find->GetIndex();
        AddRef(outIndex);
        postLoadAction(find);
        return true;
    }

    auto texRes = make_unique<TexResType>(m_device, m_descHeap);
    auto index = AllocateDescriptor();
    texRes->Load(m_upload, filename, index);

    outIndex = texRes->GetIndex();
    postLoadAction(texRes.get());
    m_texResources[index] = move(texRes);

    return true;
}

bool TextureRepository::LoadFont(const wstring& filename, size_t& outIndex)
{
    return LoadTextureResource<CFont>(filename, outIndex, [](TextureResource*) {});
}

bool TextureRepository::LoadTexture(const wstring& filename, size_t& outIndex, XMUINT2* outSize, UINT64* outGfxMemOffset)
{
    return LoadTextureResource<Texture>(filename, outIndex, 
        [outSize, outGfxMemOffset](TextureResource* res) {
            if (outSize) *outSize = res->GetSize();
            if (outGfxMemOffset) *outGfxMemOffset = res->GetGraphicMemoryOffset();
        });
}

void TextureRepository::SetTextureRenderer(function<void(size_t index, ITextureRender*)> rendererFn) noexcept
{
    m_textureRenderer = rendererFn;
}

bool TextureRepository::CreateRenderTexture(const Rectangle& targetRect, size_t& outIndex, UINT64* outGfxMemOffset)
{
    auto renderTex = make_unique<TextureRenderTarget>(m_textureRenderer, m_deviceResources, m_descHeap);

    auto format = m_deviceResources->GetBackBufferFormat();
    XMUINT2 size{ static_cast<uint32_t>(targetRect.width), static_cast<uint32_t>(targetRect.height) };
    XMINT2 position{ targetRect.x, targetRect.y };
    auto offset = AllocateDescriptor();
    if (!renderTex->Create(format, size, position, offset))
    {
        ReleaseDescriptor(offset);
        return false;
    }
    outIndex = renderTex->GetIndex();
    if (outGfxMemOffset) *outGfxMemOffset = renderTex->GetGraphicMemoryOffset();
    m_texResources[offset] = move(renderTex);

    return true;
}

void TextureRepository::ModifyRenderTexturePosition(size_t index, const XMINT2& leftTop) noexcept {
    return ToType<TextureRenderTarget*>(m_texResources[index].get())->ModifyPosition(leftTop); }

bool TextureRepository::ModifyRenderTextureSize(size_t index, const XMUINT2& size) {
    return ToType<TextureRenderTarget*>(m_texResources[index].get())->ModifySize(size); }

void TextureRepository::DrawString(size_t index, const wstring& text, const Vector2& pos, const FXMVECTOR& color) const {
    ToType<CFont*>(m_texResources[index].get())->DrawString(m_sprite, text, pos, color); }

Rectangle TextureRepository::MeasureText(size_t index, const wstring& text, const Vector2& position) {
    return ToType<CFont*>(m_texResources[index].get())->MeasureText(text, position); }

float TextureRepository::GetLineSpacing(size_t index) const noexcept {
    return ToType<CFont*>(m_texResources[index].get())->GetLineSpacing(); }

optional<vector<Rectangle>> TextureRepository::GetTextureAreaList(size_t index, const UINT32& bgColor) 
{
    auto find = FindTextureResource(index);
    if (!find) return nullopt;

    Texture* curTex = static_cast<Texture*>(find);
    vector<Rectangle> areaList;
    if (!curTex->ExtractTextureAreas(m_deviceResources, bgColor, areaList)) return nullopt;

    return areaList;
}

TextureResource* TextureRepository::FindTextureResource(const wstring& filename) const noexcept
{
    auto find = ranges::find_if(m_texResources, [&filename](const auto& res) {
        return res && res->GetFilename() == filename;
        });

    if (find != m_texResources.end())
        return find->get();

    return nullptr;
}

TextureResource* TextureRepository::FindTextureResource(size_t index) const noexcept
{
    if (index < 0 || index >= MAX_DESC) return nullptr;
    const auto& texRes = m_texResources[index];
    if (!texRes) return nullptr;
    return texRes.get();
}

void TextureRepository::Render(size_t index, const RECT& dest, const RECT* source)
{
    assert(!m_texResources.empty() && index <= m_texResources.size() - 1); //assert로 한 이유는 release일때는 조금이라도 동작을 안하게 하기 위해서이다.
    assert(m_texResources[index]->GetTypeID() != CFont::GetTypeStatic());

    //텍스춰 크기는 고정으로 함
    //dest는 화면에 보여주는 사각형(크기가 안 맞으면 강제로 늘림)
    //source는 텍스춰에서 가져오는 픽셀 사각형
    //origin은 0, 0으로 고정. 중간으로 했을 경우 늘릴때 위치가 어긋남
    //텍스춰가 늘어나면 텍스춰가 여러장일 경우 origin 값으로 설정했을때 조금씩 어긋나는 현상이 벌어진다.
    //origin을 0, 0 로 고정후 위치값을 계산해서 넘겨주는 식으로 해야겠다.

    m_sprite->Draw(m_descHeap->GetGpuHandle(index), m_texResources[index]->GetSize(), dest, source, Colors::White, 0.f);
}

void TextureRepository::DrawRenderTextures()
{
    auto texResViews = m_texResources
        | views::filter([](const unique_ptr<TextureResource>& resource) {
        return resource && resource->GetTypeID() == TextureRenderTarget::GetTypeStatic();
            });

    auto commandList = m_deviceResources->GetCommandList();
    for (auto& texRes : texResViews)
    {
        auto renderTex = static_cast<TextureRenderTarget*>(texRes.get());
        renderTex->Render(commandList, this, m_sprite);
    }
}

void TextureRepository::ReleaseTexture(size_t idx) noexcept
{
    if (idx < 0 || idx >= MAX_DESC) return;

    if (m_refCount[idx] > 0 )
    {
        m_refCount[idx]--;
        return;
    }
        
    m_deviceResources->WaitForGpu();
    Assert(m_texResources[idx]);
    m_texResources[idx] = nullptr;
    ReleaseDescriptor(idx);
}

void TextureRepository::Reset()
{
    ranges::for_each(m_texResources, [](auto& res) { res->Reset(); });
}

void TextureRepository::ReleaseDescriptor(size_t idx) noexcept
{
    assert(idx < MAX_DESC);
    
    m_freeDescIndices.push_back(idx);
}

size_t TextureRepository::AllocateDescriptor() noexcept
{
    if (!m_freeDescIndices.empty()) 
    {
        size_t idx = m_freeDescIndices.back();
        m_freeDescIndices.pop_back();
        return idx;
    }

    assert(m_nextDescIdx < MAX_DESC);
    return m_nextDescIdx++;
}
