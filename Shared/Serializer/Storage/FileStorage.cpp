#include "pch.h"
#include "FileStorage.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include <fstream>

unique_ptr<ostream> FileStorage::OpenWrite(const wstring& filename)
{
    auto path = GetResourceFullFilenameW(filename);
    auto file = make_unique<ofstream>(path);
    if (!file->is_open())
        return nullptr;
    return file;
}

unique_ptr<istream> FileStorage::OpenRead(const wstring& filename)
{
    auto path = GetResourceFullFilenameW(filename);
    auto file = make_unique<ifstream>(path);
    if (!file->is_open())
        return nullptr;
    return file;
}

unique_ptr<IJsonStorage> FileStorage::Clone() const
{
    return unique_ptr<FileStorage>(new FileStorage(*this));
}

inline static bool IsJsonFile(const wstring& filename) { return filesystem::path(filename).extension() == ".json"; }
bool FileStorage::Write(const wstring& filename, const nlohmann::ordered_json& data)
{
    ReturnIfFalse(IsJsonFile(filename));
    auto file = OpenWrite(filename);
    if (!file) return false;

    (*file) << data.dump(4);
    return true;
}

bool FileStorage::Read(const std::wstring& filename, nlohmann::json& outData)
{
    ReturnIfFalse(IsJsonFile(filename));
    auto file = OpenRead(filename);
    if (!file) return false;

    outData = nlohmann::json::parse(*file);
    return true;
}