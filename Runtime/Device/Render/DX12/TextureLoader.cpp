#include "pch.h"
#include "TextureLoader.h"
#include "ImageData.h"
#include <wincodec.h>

using Microsoft::WRL::ComPtr;

TextureLoader::~TextureLoader() = default;
TextureLoader::TextureLoader()
{
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, 
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_wicFactory));
}

ImageData TextureLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    // WIC으로 이미지 디코딩 (PNG, JPEG등)
    ComPtr<IWICStream> stream;
    m_wicFactory->CreateStream(&stream);
    stream->InitializeFromMemory(reinterpret_cast<BYTE*>(buffer.data()), static_cast<DWORD>(buffer.size()));

    ComPtr<IWICBitmapDecoder> decoder;
    m_wicFactory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder);

    ComPtr<IWICBitmapFrameDecode> frame;
    decoder->GetFrame(0, &frame);

    // 픽셀 포맷 변환
    ComPtr<IWICFormatConverter> converter;
    m_wicFactory->CreateFormatConverter(&converter);

    converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.f,
        WICBitmapPaletteTypeCustom
    );

    UINT width, height;
    converter->GetSize(&width, &height);

    ImageData img;
    img.width = width;
    img.height = height;
    img.stride = width * 4;
    img.pixels.resize(width * height * 4);

    converter->CopyPixels(nullptr, img.stride, static_cast<UINT>(img.pixels.size()), img.pixels.data());

    return img;
}