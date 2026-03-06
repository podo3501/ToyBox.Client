#include "pch.h"
#include "ResourceManager.h"
#include "ResourceStream.h"
#include "Platform/Utils/PathUtils.h"
#include <fstream>

ResourceManager::~ResourceManager() = default;
ResourceManager::ResourceManager(const filesystem::path& path) :
	m_path{ path }
{}

bool ResourceManager::Write(const filesystem::path& filename, const Core::ByteBuffer& buffer) noexcept
{
	auto absolutePath = MakeAbsolutePath(m_path, filename);
	ofstream file(absolutePath, ios::binary | ios::trunc);
	if (!file)
		return false;

	if (!buffer.empty())
	{
		file.write(
			reinterpret_cast<const char*>(buffer.data()),
			static_cast<streamsize>(buffer.size()));

		if (!file)
			return false;
	}

	return true;
}

bool ResourceManager::Read(const filesystem::path& filename, Core::ByteBuffer& outBuffer) const noexcept
{
	outBuffer.clear();

	auto absolutePath = MakeAbsolutePath(m_path, filename);
	ifstream file(absolutePath, ios::binary | ios::ate);
	if (!file)
		return false;

	const streamsize size = file.tellg();
	if (size <= 0)
		return false;

	file.seekg(0, ios::beg);

	outBuffer.resize(static_cast<size_t>(size));

	if (!file.read(reinterpret_cast<char*>(outBuffer.data()), size))
	{
		outBuffer.clear();
		return false;
	}

	return true;
}

bool ResourceManager::WriteText(const filesystem::path& filename, const string& text) noexcept
{
	Core::ByteBuffer buffer;
	buffer.resize(text.size());
	if (!text.empty())
		memcpy(buffer.data(), text.data(), text.size());

	return Write(filename, buffer);
}

bool ResourceManager::ReadText(const filesystem::path& filename, string& outText) const noexcept
{ 
	Core::ByteBuffer buffer;
	if (!Read(filename, buffer)) 
		return false; 

	outText.assign(reinterpret_cast<const char*>(buffer.data()), buffer.size());
	return true; 
}

filesystem::path ResourceManager::MakeResourceFilePath(const filesystem::path& filename) const noexcept
{
	return MakeAbsolutePath(m_path, filename);
}

unique_ptr<IResourceStream> ResourceManager::CreateReadStream(const filesystem::path& filename)
{
	auto absolutePath = MakeAbsolutePath(m_path, filename);
	auto stream = make_unique<ResourceStream>(absolutePath);

	if (stream->Size() == 0)
		return nullptr;

	return stream;
}

///////////////////////////////////////////

unique_ptr<IResourceManager> CreateResourceManager(const filesystem::path& path)
{
	return make_unique<ResourceManager>(path);
}