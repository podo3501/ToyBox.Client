#pragma once
#include "IResourceStream.h"

class ResourceStream : public IResourceStream
{
public:
    ~ResourceStream();
    explicit ResourceStream(const filesystem::path& path);
    virtual unique_ptr<IResourceStream> Clone() const override;
    virtual size_t Read(span<std::byte> buffer) override;
    virtual bool Seek(size_t pos) noexcept override;
    virtual size_t Tell() const noexcept override;
    virtual size_t Size() const noexcept override;
    virtual bool IsOpen() const noexcept override;
    virtual bool Eof() const noexcept override;
    
private:
    filesystem::path m_filePath;
    FILE* m_file = nullptr;
    size_t m_size = 0;
};