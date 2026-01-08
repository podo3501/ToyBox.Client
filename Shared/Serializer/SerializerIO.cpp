#include "pch.h"
#include "SerializerIO.h"

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