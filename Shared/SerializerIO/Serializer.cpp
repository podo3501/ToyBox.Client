#include "pch.h"
#include "Serializer.h"

Serializer::~Serializer() = default;
Serializer::Serializer() noexcept :
m_wCurrent{ &m_write },
m_rCurrent{ &m_read }
{}

Serializer::Serializer(nlohmann::ordered_json& write) noexcept { m_wCurrent = &write; }
Serializer::Serializer(const nlohmann::json& read) noexcept { m_rCurrent = const_cast<nlohmann::json*>(&read); }

bool Serializer::IsWrite()
{
    if (!m_rCurrent || m_rCurrent->empty()) return true; //읽는게 아니면 쓰는 것이다.
    return false;
}