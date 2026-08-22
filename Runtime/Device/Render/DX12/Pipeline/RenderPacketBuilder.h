#pragma once

struct SceneViewData;
struct ViewPacket;
class TextSystem;

std::shared_ptr<ViewPacket> BuildViewPacket(SceneViewData&& view, TextSystem& textSystem);