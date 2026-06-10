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

    float cameraPosition[3];
    float cameraPadding{ 0.f };

    float lightDirection[3];
    float lightIntensity;

    float lightColor[3];
    uint32_t shadowTextureIndex;
};
CHECK_ALIGN16(MeshFrameCB);
