#include "pch.h"
#include "MeshJsonLoader.h"
#include "Platform/Serializer/Serializer.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

struct JsonVec2
{
    float x, y;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("x", x);
        serializer.Process("y", y);
    }
};

struct JsonVec3
{
    float x, y, z;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("x", x);
        serializer.Process("y", y);
        serializer.Process("z", z);
    }
};

struct JsonVertex
{
    JsonVec3 position;
    JsonVec3 normal;
    JsonVec2 uv;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("position", position);
        serializer.Process("normal", normal);
        serializer.Process("uv", uv);
    }
};

struct JsonMesh
{
    std::vector<JsonVertex> vertices;
    std::vector<uint32_t> indices;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("vertices", vertices);
        serializer.Process("indices", indices);
    }
};

static MeshAsset::Vertex ConvertToVertex(const JsonVertex& v)
{
    return {
        v.position.x, v.position.y, v.position.z,
        v.normal.x, v.normal.y, v.normal.z,
        v.uv.x, v.uv.y };
}

shared_ptr<Asset> MeshJsonLoader::LoadFromMemory(const Core::ByteBuffer& buffer)
{
	nlohmann::json rData = nlohmann::json::parse(buffer.begin(), buffer.end());

    JsonMesh jsonMesh;
	DeserializeClass(rData, jsonMesh);

    auto mesh = std::make_shared<MeshAsset>();
    mesh->vertices.reserve(jsonMesh.vertices.size());

    for (const auto& v : jsonMesh.vertices)
        mesh->vertices.push_back(ConvertToVertex(v));
    mesh->indices = std::move(jsonMesh.indices);

	return mesh;
}

unique_ptr<IAssetLoader> CreateMeshJsonLoader()
{
	return make_unique<MeshJsonLoader>();
}