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
	ShaderRegisterDesc Build(ShadingModel model, std::shared_ptr<ShaderAsset> asset);
}
