#pragma once

struct IResourceStream
{
	virtual ~IResourceStream() = default;
	virtual size_t Read(span<std::byte> buffer) = 0;
	virtual bool Seek(size_t pos) noexcept = 0;
	virtual size_t Tell() const noexcept = 0;
	virtual size_t Size() const noexcept = 0;
	virtual bool IsOpen() const noexcept = 0;
	virtual bool Eof() const noexcept = 0;
};
