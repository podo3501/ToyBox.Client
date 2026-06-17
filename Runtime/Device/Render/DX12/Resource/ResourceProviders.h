#pragma once
#include "Texture/TextureProvider.h"
#include "Mesh/MeshProvider.h"
#include "Material/MaterialProvider.h"
#include "Shader/ShaderProvider.h"

class ResourceProviders
{
public:
	~ResourceProviders();
	ResourceProviders(Device& device);

private:
	Device& m_device;

	//TextureProvider m_texProvider;
	//MeshProvider m_meshProvider;
	//MaterialProvider m_matProvider;
	//ShaderProvider m_shaderProvider;
};
