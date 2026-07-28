#pragma once
#include "SurfaceMaterialDesc.h"

enum class PhongTextureSlot : uint32_t
{
    Albedo,
    Normal, //specular 텍스쳐를 꼭 쓰고 싶다면 노멀 알파채널에 값을 넣어놓고 그걸 가져다 쓰자.
    Count
};

constexpr TextureSlot Resolve(PhongTextureSlot s)
{
    return static_cast<TextureSlot>(s);
}

struct PhongSurface
{
    float normal{ 1.0f };    // 1. Normal Map의 강도 조절 (PBR과 동일)
    float shininess{ 3.0f }; // 2. 하이라이트 동그라미의 크기/선명도 (대체로 1.0 ~ 256.0)
    float specular{ 0.05f };  // 3. 하이라이트의 전체적인 밝기/세기 (0.0 ~ 1.0)
    float ambient{ 0.015f };   // 4. 환경광/기본 음영 밝기 (선택 사항, 0.0 ~ 1.0)

    bool operator==(const PhongSurface&) const = default;
    size_t GetHash() const { return Core::HashOf(normal, shininess, specular, ambient); }
};

struct PhongMaterialDesc : public SurfaceMaterialDesc
{
    PhongSurface surf{};

    PhongMaterialDesc()
    {
        surfType = SurfaceType::Phong;
        pipelineState = PipelineLibrary::Get(
            RegistryShader::Phong,
            RasterPreset::Default,
            PrimitiveTopologyType::Triangle);
    }

    bool operator==(const PhongMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(SurfaceMaterialDesc::GetHash(), surf.GetHash()); }
};