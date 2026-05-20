#include "pch.h"
#include "ShaderRepository.h"
#include "IShaderSystem.h"
#include "Service/Render/Desc/RenderState.h"

ShaderRepository::~ShaderRepository() = default;
ShaderRepository::ShaderRepository(IShaderSystem* shaderSystem) :
	m_shaderSystem{ shaderSystem }
{}

bool ShaderRepository::RegisterShader(
	std::filesystem::path path, 
	ShaderID shaderID,
	function<shared_ptr<ShaderAsset>(const filesystem::path&)> loader)
{
	if (m_registered.contains(shaderID))
		return true;

	shared_ptr<ShaderAsset> asset = loader(path);
	if (!asset)
		return false;

	if (asset->hlslSource.empty())
		return false;

	ReturnIfFalse(m_shaderSystem->Register(shaderID, asset));

	m_registered.insert(shaderID);
	return true;
}
