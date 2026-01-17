#include "pch.h"
#include "FileStorage.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include <fstream>

static unique_ptr<ostream> OpenWrite(const wstring& filename)
{
    auto path = GetResourceFullFilenameW(filename);
    auto file = make_unique<ofstream>(path);
    if (!file->is_open())
        return nullptr;
    return file;
}

static unique_ptr<istream> OpenRead(const wstring& filename)
{
    auto path = GetResourceFullFilenameW(filename);
    auto file = make_unique<ifstream>(path);
    if (!file->is_open())
        return nullptr;
    return file;
}

inline static bool IsJsonFile(const wstring& filename) { return filesystem::path(filename).extension() == ".json"; }
bool FileStorage::Write(const wstring& filename, const nlohmann::json& data)
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

unique_ptr<IJsonStorage> CreateJsonStorage()
{
    return make_unique<FileStorage>();
}