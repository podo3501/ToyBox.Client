#include "pch.h"
#include "JsonFileIO.h"
#include <fstream>
#include "Core/Service/EnvironmentLocator.h"

static bool IsJsonExtension(const wstring& filename) { return filesystem::path(filename).extension() == L".json"; }

template <typename Stream>
unique_ptr<Stream> CreateFileStream(const wstring& filename)
{
    if(!IsJsonExtension(filename)) return nullptr;

    auto path = GetResourceFullFilenameW(filename);
    auto file = make_unique<Stream>(path);
    if (!file->is_open())
        return nullptr;
    return file;
}

bool JsonFileIO::Write(const wstring& filename, const nlohmann::json& data)
{
    auto file = CreateFileStream<ofstream>(filename);
    if (!file) return false;

    (*file) << data.dump(4);
    return true;
}

bool JsonFileIO::Read(const std::wstring& filename, nlohmann::json& outData)
{
    auto file = CreateFileStream<ifstream>(filename);
    if (!file) return false;

    outData = nlohmann::json::parse(*file);
    return true;
}