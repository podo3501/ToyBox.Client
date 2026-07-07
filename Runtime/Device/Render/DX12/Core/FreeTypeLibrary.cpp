#include "pch.h"
#include "FreeTypeLibrary.h"

FreeTypeLibrary::~FreeTypeLibrary() 
{ 
    FT_Done_FreeType(m_library); 
}

FreeTypeLibrary::FreeTypeLibrary()
{
    FT_Error err = FT_Init_FreeType(&m_library);
    Assert(err == FT_Err_Ok);
}
