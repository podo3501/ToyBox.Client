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
    float cameraPosition[3];
    float cameraPadding{ 0.f };
    float lightDirection[3];
    float lightIntensity;
    float lightColor[3];
    float lightPadding{ 0.f };
};
CHECK_ALIGN16(MeshFrameCB);
