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

struct CameraCB
{
    float cameraPosition[3];
    float cameraPadding{ 0.f };
};

struct LightingCB
{
    float lightDirection[3];
    float lightIntensity;
    float lightColor[3];
    float lightPadding{ 0.f };
};

struct MeshFrameCB 
{ 
    FrameCB frame;
    CameraCB camera;
    LightingCB lighting; 
};
CHECK_ALIGN16(MeshFrameCB);
