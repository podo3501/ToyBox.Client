#pragma once

struct IShaderSystem;
struct ShaderAsset;
enum class ShaderID;

class ShaderRepository
{
public:
    ~ShaderRepository();
    explicit ShaderRepository(IShaderSystem* shaderSystem);

    bool RegisterShader(
        std::filesystem::path path, 
        ShaderID shaderID,
        function<shared_ptr<ShaderAsset>(const filesystem::path&)> loader);

private:
    IShaderSystem* m_shaderSystem{ nullptr };

    unordered_set<ShaderID> m_registered;
};
