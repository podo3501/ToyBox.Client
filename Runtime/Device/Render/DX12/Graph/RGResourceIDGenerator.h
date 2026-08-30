#pragma once
#include "RGTypes.h"

class RGResourceIDGenerator
{
public:
    void SetStartIndex(RGResourceID startIndex) noexcept
    {
        m_next = startIndex;
    }

    RGResourceID Generate() noexcept 
    { 
        if (m_next == InvalidRGID)
        {
            Assert(false); //RGResourceID의 최대 범위를 넘어섰다.
            return InvalidRGID;
        }

        return m_next++; 
    }

private:
    RGResourceID m_next{ 0 };
};

//RGResourceID가 uint32_t 라서 계속 증가해도 괜찮지만, 혹시 여기서 문제가 생겨서 넘어가게 되면 
//IndexAllocator 같은 걸로 해서 ResourceID를 다 쓰면 지워주게끔 하면 된다.