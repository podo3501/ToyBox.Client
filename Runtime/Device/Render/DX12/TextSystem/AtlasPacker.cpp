#include "pch.h"
#include "AtlasPacker.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

AtlasPacker::~AtlasPacker() { Reset(); }
AtlasPacker::AtlasPacker(const Size& textureSize)
{
    Initialize(textureSize); //?!? 일단은 생성자에서 initialize 하지만 나중에 이 부분을 호출식으로 고치던지 수정해야 한다.
}

void AtlasPacker::Initialize(const Size& textureSize)
{
    Reset();

    m_textureSize = textureSize;

    m_context = new stbrp_context();
    m_nodes.resize(textureSize.width); // Skyline 알고리즘은 너비(width)만큼의 노드 공간이 필요합니다.
    
    stbrp_init_target(
        m_context, 
        textureSize.width, 
        textureSize.height, 
        m_nodes.data(), 
        static_cast<int>(m_nodes.size())); // stb 패커 초기화
}

std::pair<int, int> AtlasPacker::AllocateRect(int width, int height)
{
    assert(m_context && "Packer is not initialized!");

    stbrp_rect rect{};
    rect.w = width;
    rect.h = height;
    
    stbrp_pack_rects(m_context, &rect, 1); // 사각형 1개 패킹 시도
    
    if (rect.was_packed) // rect.was_packed가 true라면 성공적으로 자리를 찾은 것입니다.
        return { rect.x, rect.y };

    // 공간이 부족하여 패킹에 실패한 경우 (-1, -1) 반환 (아틀라스가 가득 참)
    // 호출부(FontAtlasManager)에서 이를 받아 멀티 페이지를 파거나 에러 처리를 해야 합니다.
    return { -1, -1 };
}

void AtlasPacker::Reset()
{
    if (m_context)
    {
        delete m_context;
        m_context = nullptr;
    }
    m_nodes.clear();
}