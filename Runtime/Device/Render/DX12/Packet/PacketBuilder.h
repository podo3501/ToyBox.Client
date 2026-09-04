#pragma once

struct FramePacket;
struct SceneFrameData;
class TextSystem;
class TransientMeshProvider;
struct Size;

FramePacket BuildPacket(
	SceneFrameData& frame,
	TextSystem& textSystem,
	TransientMeshProvider& meshProvider,
	const Size& screenSize);