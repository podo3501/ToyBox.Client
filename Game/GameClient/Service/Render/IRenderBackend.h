#pragma once

struct IRenderBackend
{

};

unique_ptr<IRenderBackend> CreateRenderBackend();