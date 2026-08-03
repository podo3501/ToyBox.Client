#include "pch.h"
#include "SphericalHarmonicsLoader.h"

static bool ParseVector3Line(const std::string& line, Core::Vector3& outVec)
{
    auto openParen = line.find('(');
    auto closeParen = line.find(')');
    if (openParen == std::string::npos || closeParen == std::string::npos || closeParen <= openParen)
        return false;

    std::string inner = line.substr(openParen + 1, closeParen - openParen - 1);

    // 쉼표를 공백으로 바꿔서 스트림으로 편하게 파싱
    for (char& c : inner)
    {
        if (c == ',')
            c = ' ';
    }

    std::istringstream stream(inner);
    float x, y, z;
    if (!(stream >> x >> y >> z))
        return false;

    outVec = Core::Vector3(x, y, z);
    return true;
}

std::shared_ptr<SphericalHarmonicsAsset> SphericalHarmonicsLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    std::string text(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    std::istringstream stream(text);

    auto asset = std::make_shared<SphericalHarmonicsAsset>();

    std::string line;
    size_t index = 0;

    while (std::getline(stream, line) && index < asset->coefficients.size())
    {
        // 빈 줄이나 괄호가 없는 줄은 무시
        if (line.find('(') == std::string::npos)
            continue;

        Core::Vector3 coeff;
        if (!ParseVector3Line(line, coeff))
        {
            Assert(false); // 예상한 포맷과 다른 줄 -> sh.txt 포맷이 바뀌었는지 확인 필요
            return nullptr;
        }

        asset->coefficients[index] = coeff;
        ++index;
    }

    if (index != asset->coefficients.size())
    {
        Assert(false); // 9개를 못 채움 -> 파일이 잘렸거나 포맷이 다름
        return nullptr;
    }

	return asset;
}

unique_ptr<IAssetLoader> CreateSphericalHarmonicsLoader()
{
	return make_unique<SphericalHarmonicsLoader>();
}
