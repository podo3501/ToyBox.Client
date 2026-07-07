#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

class FreeTypeLibrary
{
public:
    ~FreeTypeLibrary();
    FreeTypeLibrary();

    FT_Library Get() const { return m_library; }

private:
    FT_Library m_library{ nullptr };
};
