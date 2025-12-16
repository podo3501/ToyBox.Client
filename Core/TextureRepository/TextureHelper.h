#pragma once

namespace DX
{
    class DeviceResources;
}

bool ExtractAreas(DX::DeviceResources* deviceRes, ID3D12Resource* texRes, const UINT32& bgColor, vector<Rectangle>& outList);