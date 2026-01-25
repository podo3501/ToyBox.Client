#include "pch.h"
#include "TextureFontInfo.h"
#include "Renderer/Public/IRenderer.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include "Shared/Serializer/Serializer.h"

TextureFontInfo::~TextureFontInfo() = default;
TextureFontInfo::TextureFontInfo() noexcept = default;
TextureFontInfo::TextureFontInfo(const TextureFontInfo& other) noexcept :
    filename{ other.filename }
{}

TextureFontInfo::TextureFontInfo(const wstring& _filename) noexcept :
    filename{ _filename }
{}

bool TextureFontInfo::operator==(const TextureFontInfo& o) const noexcept
{
    return (filename == o.filename);
}

bool TextureFontInfo::LoadResource(ITextureLoad* load)
{
    if (filename.empty()) return false;
    Release();

    size_t index{ 0 };
    ReturnIfFalse(load->LoadFont(GetResourceFullFilenameW(filename), index));
    SetIndex(index);
    SetTextureLoader(load);

    return true;
}

void TextureFontInfo::Serialize(Serializer& serializer)
{
    serializer.Process("Filename", filename);
}