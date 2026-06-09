#pragma once
#include "RGTypes.h"

class RenderGraph;

class FrameEndGraphBuilder
{
public:
    ~FrameEndGraphBuilder() = default;
    FrameEndGraphBuilder() = delete;
    FrameEndGraphBuilder(RGHandle hBb, RGHandle hShadow);
    void Build(RenderGraph& graph);

private:
    RGHandle m_hBb;
    RGHandle m_hShadow;
};
