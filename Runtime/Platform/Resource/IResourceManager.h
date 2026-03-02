#pragma once

struct IResourceManager
{
	virtual ~IResourceManager() = default;
	virtual bool Write(const filesystem::path& filename, const Core::ByteBuffer& buffer) noexcept = 0;
	virtual bool Read(const filesystem::path& filename, Core::ByteBuffer& outBuffer) const noexcept = 0;
	virtual bool WriteText(const filesystem::path& filename, const string& text) noexcept = 0;
	virtual bool ReadText(const filesystem::path& filename, string& outText) const noexcept = 0;

	virtual filesystem::path MakeResourceFilePath(const filesystem::path& filename) const noexcept = 0;
};

unique_ptr<IResourceManager> CreateResourceManager(const filesystem::path& path);