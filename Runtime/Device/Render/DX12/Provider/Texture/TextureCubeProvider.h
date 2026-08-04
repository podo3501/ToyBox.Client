#pragma once
#include "TextureCubeLoadRequest.h"
#include "TextureCubeCreateGraphBuilder.h"
#include <queue>

struct TextureCubeLoadRequest;
class TextureCubeResource;

class TextureCubeProvider
{
public:
    ~TextureCubeProvider();
    TextureCubeProvider(TextureCubeCreateGraphBuilder create) noexcept;

    std::shared_ptr<TextureCubeResource> CreateResource();

    bool LoadResource(
        std::shared_ptr<TextureCubeResource> resource,
        std::shared_ptr<TextureCubeAsset> asset);

    void Update(size_t uploadBudgetBytes);

private:
    TextureCubeCreateGraphBuilder m_createBuilder;
    std::queue<TextureCubeLoadRequest> m_pending;
};