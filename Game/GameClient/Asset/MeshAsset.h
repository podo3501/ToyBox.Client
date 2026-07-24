#pragma once
#include <vector>
#include "AssetData.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Foundation/Color.h"

enum class VertexFormat
{
    Mesh,
    UI,
    Grid,
};

struct MeshVertex
{
    Core::Vector3 position;
    Core::Vector3 normal;
    Core::Vector2 uv;
    Core::Vector3 tangent;
};

enum class UIRenderMode : uint32_t
{
    UI = 0,
    BitmapText = 1,
    MTSDF = 2
};

struct UITextProps
{
    float sdfPxRange{ 0.f };
    uint32_t params{ 0 }; //pack된 값들 순서 : weight, outlinecolor
};

struct UIVertex
{
    Core::Vector3 position;
    Core::Color color;
    Core::Vector2 uv;
    uint32_t textureIndex{ 0 }; //Bindless SRV Heap Index. 일단은 ui는 cb로 인덱스를 가지고 오는 걸로 사용한다.
    UIRenderMode mode{ UIRenderMode::UI };
    UITextProps textProps;
};

struct GridVertex
{
    Core::Vector3 position;
    Core::Color color;
};

struct MeshAsset : public AssetData
{
    CORE_DECLARE_TYPE(MeshAsset)

    VertexFormat format;

    uint32_t vertexStride{ 0 }; //generic하게 byte로 바뀌었기 때문에 보폭을 저장해야 한다.
    uint32_t vertexCount{ 0 };

    std::vector<std::byte> vertices;
    std::vector<uint32_t> indices;

    template<typename T>
    void SetVertices(const std::vector<T>& src)
    {
        vertexStride = sizeof(T);
        vertexCount = static_cast<uint32_t>(src.size());

        vertices.resize(sizeof(T) * src.size());
        memcpy(vertices.data(), src.data(), vertices.size());
    }
};