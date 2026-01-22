#include "pch.h"
#include "Font.h"

CFont::~CFont() = default;
CFont::CFont(ID3D12Device* device, DescriptorHeap* descHeap) noexcept :
    TextureResource{ device, descHeap }
{}

void CFont::Load(ResourceUploadBatch* upload, const wstring& filename, size_t index)
{
    m_font = make_unique<SpriteFont>(m_device, *upload, filename.c_str(),
        m_srvDescriptors->GetCpuHandle(index), m_srvDescriptors->GetGpuHandle(index));
    SetFilename(filename);
    SetIndex(index);
}

inline static Rectangle ConvertRectangle(const RECT& rect) noexcept
{
    Rectangle rectangle;
    rectangle.x = static_cast<long>(rect.left);
    rectangle.y = static_cast<long>(rect.top);
    rectangle.width = static_cast<long>(rect.right - rect.left);
    rectangle.height = static_cast<long>(rect.bottom - rect.top);

    return rectangle;
}

Rectangle CFont::MeasureText(const wstring& text, const Vector2& position) const
{
    const RECT& rect = m_font->MeasureDrawBounds(text.c_str(), position, false);
    return ConvertRectangle(rect);
}

void CFont::DrawString(SpriteBatch* spriteBatch, const wstring& text, const Vector2 pos, const FXMVECTOR& color) const
{
    m_font->DrawString(spriteBatch, text.c_str(), pos, color, 0.f, { 0.f, 0.f }, 1.0f);
}