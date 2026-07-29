#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Color.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

struct IFontResource;
struct IMeshResource;
struct IMaterialResource;
class TextureResource;

struct DrawItem
{
    std::shared_ptr<IMeshResource> mesh;
    std::shared_ptr<IMaterialResource> material;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
};

struct DrawUIItem : public DrawItem
{
    Core::Vector4 uvTransform{ 0.f, 0.f, 1.f, 1.f };
};

struct DrawTextRun
{
    std::vector<char32_t> codePoints; //?!? 변수명을 바꿨으면 좋겠다. 글자들을 utf-8에서 utf-32로 바꾼건데 글자인지 뭔지 잘 모르겠네.
    TextStyle style;
    uint32_t lineIndex{ 0 }; // 이 run이 몇 번째 줄에 속하는지 (절대값)
};

struct DrawTextItem
{
    std::shared_ptr<IFontResource> fontRes;
    TextRenderMode mode;
    std::vector<DrawTextRun> runs;
    uint32_t fontSize{ 0 };
    Core::Vector2 position{};
    Core::Vector2 size{};
};

struct DrawInspectItem
{
    std::shared_ptr<TextureResource> texture;
};

struct DebugPacket
{
    std::span<DrawInspectItem> images;
};

struct DrawPacket
{
    std::span<DrawItem> surface;
    std::span<DrawItem> debugSurface;
    std::span<DrawUIItem> ui;

    DebugPacket debug;
};