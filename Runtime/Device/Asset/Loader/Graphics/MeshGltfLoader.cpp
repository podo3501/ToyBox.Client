#include "pch.h"
#include "MeshGltfLoader.h"
#include "GameClient/Asset/BinaryAsset.h"
#include "Core/Foundation/Cast.hpp"

#pragma warning(push)
#pragma warning(disable : 4996)
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#pragma warning(pop)

static bool ReadAccessorFloats(const cgltf_accessor* accessor, float* dst, size_t componentCount, size_t dstStride)
{
    if (!accessor) return false;

    size_t count = accessor->count;
    for (size_t i = 0; i < count; ++i)
    {
        float temp[4] = {};
        if (!cgltf_accessor_read_float(accessor, i, temp, componentCount))
            return false;

        float* dstElem = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(dst) + i * dstStride);
        for (size_t c = 0; c < componentCount; ++c)
            dstElem[c] = temp[c];
    }
    return true;
}

// glTF는 TANGENT를 없이 export하는 경우가 흔해서, 없으면 임의의 기본값으로 채움
// (필요하면 나중에 노멀 기반으로 계산하는 로직으로 교체 가능)
static void FillDefaultTangents(std::vector<MeshVertex>& vertices)
{
    for (auto& v : vertices)
        v.tangent = { 1.0f, 0.0f, 0.0f };
}

MeshGltfLoader::MeshGltfLoader(IAssetProvider* assetProvider) noexcept
    : m_assetProvider{ assetProvider }
{}

std::shared_ptr<MeshAsset> MeshGltfLoader::LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer)
{
    cgltf_options options{};
    cgltf_data* data = nullptr;

    // JSON 파싱만 함 - 아직 버퍼(.bin)나 이미지는 안 건드림
    cgltf_result result = cgltf_parse(&options, buffer.data(), buffer.size(), &data);
    if (result != cgltf_result_success || !data)
        return nullptr;

    std::vector<Core::ByteBuffer> loadedBuffers;
    loadedBuffers.reserve(data->buffers_count);

    for (size_t i = 0; i < data->buffers_count; ++i)
    {
        cgltf_buffer& buf = data->buffers[i];

        if (!buf.uri) // embedded(.glb)면 uri가 없고 cgltf가 이미 처리했을 수 있음
            continue;

        // "marble_bust_01.bin" 같은 상대경로를 .gltf 파일 기준 형제 경로로 해석
        auto asset = m_assetProvider->Load(Core::GetTypeID<BinaryAsset>(), resID.MakeSibling(buf.uri));
        if (!asset)
        {
            cgltf_free(data);
            return nullptr;
        }

        auto binary = Core::Cast<BinaryAsset>(asset);
        loadedBuffers.emplace_back(std::move(binary->buffer));
        buf.data = loadedBuffers.back().data();
        buf.size = loadedBuffers.back().size();
    }

    // 이제 정점/인덱스 accessor를 읽을 준비가 됐음 (이전과 동일한 로직)
    if (data->meshes_count == 0 || data->meshes[0].primitives_count == 0)
    {
        Assert(false);
        cgltf_free(data);
        return nullptr;
    }

    const cgltf_primitive& prim = data->meshes[0].primitives[0];

    const cgltf_accessor* posAccessor = nullptr;
    const cgltf_accessor* normalAccessor = nullptr;
    const cgltf_accessor* uvAccessor = nullptr;
    const cgltf_accessor* tangentAccessor = nullptr;

    for (size_t i = 0; i < prim.attributes_count; ++i)
    {
        const cgltf_attribute& attr = prim.attributes[i];
        switch (attr.type)
        {
        case cgltf_attribute_type_position: posAccessor = attr.data; break;
        case cgltf_attribute_type_normal:   normalAccessor = attr.data; break;
        case cgltf_attribute_type_texcoord: uvAccessor = attr.data; break;
        case cgltf_attribute_type_tangent:  tangentAccessor = attr.data; break;
        default: break; // materials/textures/images는 아예 신경 안 씀
        }
    }

    if (!posAccessor)
    {
        Assert(false);
        cgltf_free(data);
        return nullptr;
    }

    size_t vertexCount = posAccessor->count;
    std::vector<MeshVertex> vertices(vertexCount);

    ReadAccessorFloats(posAccessor, &vertices[0].position.x, 3, sizeof(MeshVertex));
    if (normalAccessor) ReadAccessorFloats(normalAccessor, &vertices[0].normal.x, 3, sizeof(MeshVertex));
    if (uvAccessor) ReadAccessorFloats(uvAccessor, &vertices[0].uv.x, 2, sizeof(MeshVertex));

    if (tangentAccessor)
    {
        for (size_t i = 0; i < vertexCount; ++i)
        {
            float temp[4] = {};
            cgltf_accessor_read_float(tangentAccessor, i, temp, 4);
            vertices[i].tangent = { temp[0], temp[1], temp[2] };
        }
    }
    else
    {
        FillDefaultTangents(vertices); // 지금 파일 케이스처럼 TANGENT 없는 경우
    }

    std::vector<uint32_t> indices;
    if (prim.indices)
    {
        indices.resize(prim.indices->count);
        for (size_t i = 0; i < prim.indices->count; ++i)
            indices[i] = static_cast<uint32_t>(cgltf_accessor_read_index(prim.indices, i));
    }

    cgltf_free(data);

    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Mesh;
    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}

unique_ptr<IAssetLoader> CreateMeshGltfLoader(IAssetProvider* assetProvider)
{
    return make_unique<MeshGltfLoader>(assetProvider);
}