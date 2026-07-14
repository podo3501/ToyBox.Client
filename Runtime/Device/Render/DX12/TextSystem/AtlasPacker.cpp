#include "pch.h"
#include "AtlasPacker.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

AtlasPacker::~AtlasPacker() = default;
AtlasPacker::AtlasPacker() = default;

void AtlasPacker::Initialize(const Size& textureSize)
{
    Reset();

    m_textureSize = textureSize;

    m_context = std::make_unique<stbrp_context>();
    m_nodes.resize(textureSize.width); // Skyline 알고리즘은 너비(width)만큼의 노드 공간이 필요합니다.
    
    stbrp_init_target(
        m_context.get(),
        textureSize.width, 
        textureSize.height, 
        m_nodes.data(), 
        static_cast<int>(m_nodes.size())); // stb 패커 초기화
}

Point AtlasPacker::AllocateRect(const Size& size)
{
    assert(m_context && "Packer is not initialized!");

    stbrp_rect rect{};
    rect.w = size.width;
    rect.h = size.height;
    
    stbrp_pack_rects(m_context.get(), &rect, 1); // 사각형 1개 패킹 시도
    
    if (rect.was_packed) // rect.was_packed가 true라면 성공적으로 자리를 찾은 것입니다.
        return { rect.x, rect.y };

    // 공간이 부족하여 패킹에 실패한 경우 (-1, -1) 반환 (아틀라스가 가득 참)
    // 호출부(FontAtlasManager)에서 이를 받아 멀티 페이지를 파거나 에러 처리를 해야 합니다.
    return { -1, -1 };
}

void AtlasPacker::Reset()
{
    if (m_context)
        m_context.reset();

    m_nodes.clear();
}