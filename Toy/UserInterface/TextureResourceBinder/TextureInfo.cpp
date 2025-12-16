#include "pch.h"
#include "TextureInfo.h"
#include "IRenderer.h"

TextureInfo::~TextureInfo()
{
    Release();
}

TextureInfo::TextureInfo() noexcept :
    m_texLoader{ nullptr }
{}

TextureInfo::TextureInfo(const TextureInfo& other) noexcept : 
    m_texLoader{ other.m_texLoader },
    m_index{ other.m_index },
    m_gfxOffset{ other.m_gfxOffset }
{
    AddRef();
}

TextureInfo::TextureInfo(TextureInfo&& other) noexcept :
    m_texLoader{ move(other.m_texLoader) },
    m_index{ move(other.m_index) },
    m_gfxOffset{ move(other.m_gfxOffset) }
{
    AddRef();
}

//stl은 기본이 대입 복사인데, 이때는 생성자가 호출이 안되므로 생성자에서 처리한 게 있다면 여기서도 해 줘야 한다.
TextureInfo& TextureInfo::operator=(const TextureInfo& o) noexcept
{
    Release(); //기존의 텍스쳐를 릴리즈하고
    tie(m_texLoader, m_index, m_gfxOffset) = tie(o.m_texLoader, o.m_index, o.m_gfxOffset);

    AddRef(); //새로운 텍스쳐를 Add 한다.
    return *this;
}


void TextureInfo::AddRef() noexcept
{
    if (m_texLoader && m_index)
        m_texLoader->AddRef(*m_index);
}

void TextureInfo::Release() noexcept
{
    if (m_texLoader && m_index)
    {
        m_texLoader->ReleaseTexture(*m_index);
        m_texLoader = nullptr;
        m_index = nullopt;
    }
}