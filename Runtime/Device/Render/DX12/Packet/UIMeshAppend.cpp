#include "pch.h"
#include "UIMeshAppend.h"
#include "UIBatchBuffer.h"
#include "Resource/Mesh/StaticMeshResource.h"
#include "Resource/Brush/BrushResource.h"
#include "Helpers/MathHelpers.h"
#include "GameClient/Service/Render/Definition/View/SceneFrameData.h"

static std::span<const UIVertex> AsUIVertices(const MeshAsset& asset)
{
    Assert(asset.format == VertexFormat::UI);
    Assert(asset.vertexStride == sizeof(UIVertex));

    const UIVertex* data = reinterpret_cast<const UIVertex*>(asset.vertices.data());
    return std::span<const UIVertex>(data, asset.vertexCount);
}

static void AppendUIMesh(
    UIBatchBuffer& buffer,
    std::span<const UIVertex> srcVertices,
    std::span<const uint32_t> srcIndices,
    const Core::Matrix& world,
    const Core::Vector4& uvTransform,
    UINT textureIndex)
{
    uint32_t baseVertex = static_cast<uint32_t>(buffer.vertices.size());
    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);

    for (const auto& v : srcVertices)
    {
        UIVertex out = v;
        DirectX::XMVECTOR p = DirectX::XMVectorSet(v.position.x, v.position.y, v.position.z, 1.0f);
        p = DirectX::XMVector3TransformCoord(p, xmWorld);

        out.position = { DirectX::XMVectorGetX(p), DirectX::XMVectorGetY(p), DirectX::XMVectorGetZ(p) };
        out.uv.x = std::lerp(uvTransform.x, uvTransform.z, v.uv.x);
        out.uv.y = std::lerp(uvTransform.y, uvTransform.w, v.uv.y);
        out.textureIndex = textureIndex;
        buffer.vertices.push_back(out);
    }

    for (uint32_t idx : srcIndices)
        buffer.indices.push_back(baseVertex + idx);
}

static void PadToQuadAlignment(UIBatchBuffer& buffer)
{
    uint32_t remainder = static_cast<uint32_t>(buffer.vertices.size()) % 4;
    if (remainder == 0)
        return;

    uint32_t padCount = 4 - remainder;
    UIVertex degenerate{}; // position 0, 렌더링에 영향 없음 (index buffer에 참조 안 되므로)
    for (uint32_t i = 0; i < padCount; ++i)
        buffer.vertices.push_back(degenerate);
}

void AppendUIItems(
    std::span<const DrawUIItem> uiItems,
    UIBatchBuffer& buffer)
{
    for (const auto& ui : uiItems)
    {
        auto* meshRes = static_cast<StaticMeshResource*>(ui.mesh.get());
        const MeshAsset* tmpl = meshRes->GetCPUTemplate();
        if (!tmpl)
            continue; //ui는 이 cpu asset을 다 들고 있다고 가정한다. 만약 다른 메쉬종류가 template를 쓰거나 ui가 불필요한 asset을 카피한다고 생각된다면 수정해야 한다.

        auto* brush = static_cast<BrushResource*>(ui.brush.get());
        Core::Vector4 uvTransform = brush->CalcUVTransform(ui.source);
        AppendUIMesh(
            buffer,
            AsUIVertices(*tmpl),
            tmpl->indices,
            ui.world,
            uvTransform,
            brush->GetTextureIndex());

        if (tmpl->vertexCount % 4 != 0)
            PadToQuadAlignment(buffer); // 글자 찍을때 셰이더에 4의 배수에서 계산하는 코드가 있기 때문에 padding으로 맞춘다.
    }
}