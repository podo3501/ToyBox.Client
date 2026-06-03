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

struct LightingCB
{
    float lightDirection[3];
    float lightIntensity;
    float lightColor[3];
    float lightPadding;
};

struct MeshFrameCB 
{ 
    FrameCB frame; 
    LightingCB lighting; 
};
