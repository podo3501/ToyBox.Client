#pragma once
#include "TextureResource.h"

class CFont : public TextureResource
{
public:
    static TextureResourceID GetTypeStatic() { return TextureResourceID::Font; }
    virtual TextureResourceID GetTypeID() const noexcept override { return GetTypeStatic(); }

    ~CFont();
    CFont() = delete;
    CFont(ID3D12Device* device, DescriptorHeap* descHeap) noexcept;

    void Load(ResourceUploadBatch* upload, const wstring& filename, size_t index);
    inline float GetLineSpacing() const noexcept { return m_font->GetLineSpacing(); }
    Rectangle MeasureText(const wstring& text, const Vector2& position) const;
    void DrawString(SpriteBatch* spriteBatch, const wstring& text, const Vector2 pos, const FXMVECTOR& color) const;

private:
    unique_ptr<SpriteFont> m_font;
};
