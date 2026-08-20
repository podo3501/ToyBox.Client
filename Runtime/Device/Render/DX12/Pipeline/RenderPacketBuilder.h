#pragma once

struct SceneViewData;
struct RenderPacket;
class TextSystem;

std::shared_ptr<RenderPacket> BuildRenderPacket(SceneViewData&& view, TextSystem& textSystem);