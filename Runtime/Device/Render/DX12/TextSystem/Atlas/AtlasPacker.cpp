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

std::optional<Point> AtlasPacker::AllocateRect(const Size& size)
{
    Assert(m_context); //Packer is not initialized!

    stbrp_rect rect{};
    rect.w = size.width;
    rect.h = size.height;
    
    stbrp_pack_rects(m_context.get(), &rect, 1); // 사각형 1개 패킹 시도
    if (!rect.was_packed) // 공간 부족으로 패킹 실패 (아틀라스가 가득 참)
        return std::nullopt;

    return Point{ rect.x, rect.y };
}

void AtlasPacker::Reset()
{
    m_context.reset();
    m_nodes.clear();
}