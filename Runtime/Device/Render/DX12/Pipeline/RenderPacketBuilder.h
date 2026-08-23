#pragma once

struct SceneViewData;
struct ViewPacket;
struct Size;
class TextSystem;

std::shared_ptr<ViewPacket> BuildViewPacket(
	SceneViewData&& view, 
	TextSystem& textSystem,
	const Size& screenSize);