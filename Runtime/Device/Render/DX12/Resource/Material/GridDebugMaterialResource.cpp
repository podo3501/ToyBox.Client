#include "pch.h"
#include "GridDebugMaterialResource.h"

GridDebugMaterialResource::GridDebugMaterialResource() : 
	DebugMaterialResource{
		PipelineLibrary::Get(
			RegistryShader::Grid,
			RasterPreset::Default,
			PrimitiveTopologyType::Line) 
	}
{}