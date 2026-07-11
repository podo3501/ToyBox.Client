#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"

struct GlyphUploadEntry // 하나의 글자를 아틀라스로 전송하기 위한 데이터 묶음
{
    std::vector<uint8_t> pixelData; // CPU 비트맵 임시 버퍼
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t packX{ 0 };
    uint32_t packY{ 0 };
};

struct TextRenderLayout;
class TaskScheduler;
class ResourceFactory;
class RenderGraph;

class FontAtlasCreateGraphBuilder
{
public:
    ~FontAtlasCreateGraphBuilder();
    FontAtlasCreateGraphBuilder() = delete;
    FontAtlasCreateGraphBuilder(
        TaskScheduler& taskScheduler,
        ResourceFactory& resourceFactory);
    
    void UploadGlyphsToAtlas(
        const Resource& atlasResource,
        const std::vector<GlyphUploadEntry>& uploads);

private:
    void BuildUploadPass(
        RenderGraph& graph,
        RGResourceID atlasResID,
        RGResourceID uploadResID,
        const std::vector<GlyphUploadEntry>& uploads,
        const std::vector<TextRenderLayout>& layouts);

private:
    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
};