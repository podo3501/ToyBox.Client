#pragma once
#include "IResourceManager.h"

class ResourceManager : public IResourceManager
{
public:
	~ResourceManager();
	ResourceManager() = delete;
	explicit ResourceManager(const filesystem::path& path);
	virtual bool Write(const filesystem::path& filename, const Core::ByteBuffer& buffer) noexcept override;
	virtual bool Read(const filesystem::path& filename, Core::ByteBuffer& outBuffer) const noexcept override;
	virtual bool WriteText(const filesystem::path& filename, const string& text) noexcept override;
	virtual bool ReadText(const filesystem::path& filename, string& outText) const noexcept override;
	virtual filesystem::path MakeResourceFilePath(const filesystem::path& filename) const noexcept override;

private:
	filesystem::path m_path;
};
