#include "pch.h"
#include "TextureFontInfo.h"
#include "IRenderer.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include "Shared/SerializerIO/SerializerIO.h"

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

void TextureFontInfo::ProcessIO(SerializerIO& serializer)
{
    serializer.Process("Filename", filename);
}