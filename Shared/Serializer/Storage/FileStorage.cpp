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