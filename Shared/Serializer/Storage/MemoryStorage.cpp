#include "pch.h"
#include "MemoryStorage.h"

MemoryStorage::MemoryStorage(const MemoryStorage& o) :
    m_files{ o.m_files }
{}

unique_ptr<IJsonStorage> MemoryStorage::Clone() const
{
    return unique_ptr<MemoryStorage>(new MemoryStorage(*this));
}

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

inline static bool IsJsonFile(const wstring& filename) { return filesystem::path(filename).extension() == ".json"; }
bool MemoryStorage::Write(const wstring& filename, const nlohmann::ordered_json& data)
{
    ReturnIfFalse(IsJsonFile(filename));
    auto file = OpenWrite(filename);
    if (!file) return false;

    (*file) << data.dump(4);
    return true;
}

bool MemoryStorage::Read(const wstring& filename, nlohmann::json& outData)
{
    ReturnIfFalse(IsJsonFile(filename));
    auto file = OpenRead(filename);
    if (!file) return false;

    outData = nlohmann::json::parse(*file);
    return true;
}