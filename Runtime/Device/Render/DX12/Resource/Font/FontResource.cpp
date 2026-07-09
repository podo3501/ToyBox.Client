#include "pch.h"
#include "FontResource.h"

FontResource::~FontResource()
{
    if (m_ftFace)
    {
        FT_Done_Face(m_ftFace);
        m_ftFace = nullptr;
    }
}

FontResource::FontResource() = default;

