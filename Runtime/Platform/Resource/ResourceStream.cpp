#include "pch.h"
#include "ResourceStream.h"

ResourceStream::~ResourceStream()
{
    if (m_file)
        fclose(m_file);
}

ResourceStream::ResourceStream(const std::filesystem::path& path)
{
#ifdef _WIN32
    FILE* file = nullptr;
    _wfopen_s(&file, path.c_str(), L"rb");
    m_file = file;
#else
    m_file = fopen(path.string().c_str(), "rb");
#endif

    if (!m_file)
        return;

    m_size = static_cast<size_t>(filesystem::file_size(path));
}

size_t ResourceStream::Read(span<std::byte> buffer)
{
    if (!m_file)
        return 0;

    return fread(buffer.data(), 1, buffer.size(), m_file);
}

bool ResourceStream::Seek(size_t pos) noexcept
{
    if (!m_file)
        return false;

#ifdef _WIN32
    return _fseeki64(m_file, static_cast<long long>(pos), SEEK_SET) == 0;
#else
    return fseeko(m_file, pos, SEEK_SET) == 0;
#endif
}

size_t ResourceStream::Tell() const noexcept
{
    if (!m_file)
        return 0;

#ifdef _WIN32
    return static_cast<size_t>(_ftelli64(m_file));
#else
    return static_cast<size_t>(ftello(m_file));
#endif
}

size_t ResourceStream::Size() const noexcept
{
    return m_size;
}

bool ResourceStream::IsOpen() const noexcept
{
    return m_file != nullptr;
}

bool ResourceStream::Eof() const noexcept
{
    return m_file ? feof(m_file) != 0 : true;
}