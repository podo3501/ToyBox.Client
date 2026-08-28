#include "pch.h"
#include "GridDebugMaterialResource.h"
#include "GameClient/Service/Render/Definition/Shader/RegistryShader.h"

GridDebugMaterialResource::GridDebugMaterialResource() : 
	DebugMaterialResource{
		PipelineLibrary::Get(
			RegistryShader::Grid,
			RasterPreset::Default,
			PrimitiveTopologyType::Line) 
	}
{}