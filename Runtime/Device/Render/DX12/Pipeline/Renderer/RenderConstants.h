#pragma once

struct ObjectCB
{
    DirectX::XMFLOAT4X4 world;
};

struct FrameCB
{
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;
};

struct MeshFrameCB 
{ 
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;
    DirectX::XMFLOAT4X4 lightViewProj;

    DirectX::XMFLOAT3 cameraPosition;
    float cameraPadding{ 0.f };

    DirectX::XMFLOAT3 lightDirection;
    float lightIntensity;

    DirectX::XMFLOAT3 lightColor;
    uint32_t shadowTextureIndex;

    uint32_t reflectionTextureIndex;
    uint32_t reflectionMipCount;
    float envPadding[2];
    DirectX::XMFLOAT4 irradianceSH[9];
};
CHECK_ALIGN16(MeshFrameCB);
