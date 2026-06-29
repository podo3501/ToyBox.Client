#pragma once
#include "GameClient/Service/Render/Desc/RenderState.h"

namespace ShaderStageBuilder
{
	ShaderStageDesc VS(std::string entry = "VSMain");
	ShaderStageDesc PS(std::string entry = "PSMain");
	ShaderStageDesc CS(std::string entry = "CSMain");
}

struct ShaderAsset;
namespace ShaderBuilder
{
	//Runtime
	ShaderDesc BuildGraphics(std::shared_ptr<ShaderAsset> asset);
	ShaderDesc BuildCompute(std::shared_ptr<ShaderAsset> asset);

	//Builtin
	BuiltinShaderDesc BuildGraphics(ShaderKey key, std::shared_ptr<ShaderAsset> asset);
	BuiltinShaderDesc BuildCompute(ShaderKey key, std::shared_ptr<ShaderAsset> asset);
}
