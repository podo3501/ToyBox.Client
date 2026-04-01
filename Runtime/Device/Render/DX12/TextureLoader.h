#pragma once
#include <wrl/client.h>

struct IWICImagingFactory;
struct ImageData;

class TextureLoader
{
public:
    ~TextureLoader();
    TextureLoader();
    ImageData LoadFromMemory(Core::ByteBuffer buffer);

private:
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
};