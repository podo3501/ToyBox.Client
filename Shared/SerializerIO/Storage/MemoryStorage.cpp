#include "pch.h"
#include "MemoryStorage.h"

unique_ptr<ostream> MemoryStorage::OpenWrite(const wstring& filename)
{
    struct MemStream : ostringstream {
        MemoryStorage* parent;
        wstring filename;
        ~MemStream() override {
            parent->m_files[filename] = str(); //stream이 사라질때 이 소멸자가 호출되면서 string 전부가 map에 저장된다.
        }
    };
    auto stream = make_unique<MemStream>();
    stream->parent = this;
    stream->filename = filename;
    return stream;
}

unique_ptr<istream> MemoryStorage::OpenRead(const wstring& filename)
{
    auto it = m_files.find(filename);
    if (it == m_files.end())
        return nullptr;
    return make_unique<istringstream>(it->second);
}