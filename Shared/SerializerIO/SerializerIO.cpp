#include "pch.h"
#include "SerializerIO.h"
#include "Shared/SerializerIO/Storage/JsonStorageLocator.h"

SerializerIO::~SerializerIO() = default;
SerializerIO::SerializerIO() noexcept :
m_wCurrent{ &m_write },
m_rCurrent{ &m_read }
{}

SerializerIO::SerializerIO(nlohmann::ordered_json& write) noexcept { m_wCurrent = &write; }
SerializerIO::SerializerIO(const nlohmann::json& read) noexcept { m_rCurrent = const_cast<nlohmann::json*>(&read); }

bool SerializerIO::IsWrite()
{
    if (!m_rCurrent || m_rCurrent->empty()) return true; //읽는게 아니면 쓰는 것이다.
    return false;
}

inline static bool IsJsonFile(const wstring& filename) { return filesystem::path(filename).extension() == ".json"; }
bool SerializerIO::Write(const wstring& filename)
{
    ReturnIfFalse(IsJsonFile(filename));
    auto file = GetJsonStorage()->OpenWrite(filename);
    if (!file)
        return false;
    auto& json = GetWrite();
    (*file) <<  json.dump(4);

    return true;
}

bool SerializerIO::Read(const wstring& filename)
{
    ReturnIfFalse(IsJsonFile(filename));
    auto file = GetJsonStorage()->OpenRead(filename);
    if (!file)
        return false;
    (*m_rCurrent) = nlohmann::json::parse(*file);
    return true;
}