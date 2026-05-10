#pragma once
#include "RGTypes.h"

class RenderGraph;

class PresentGraphBuilder
{
public:
    ~PresentGraphBuilder() = default;
    PresentGraphBuilder() = delete;
    PresentGraphBuilder(RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    RGHandle m_hBb;
};
