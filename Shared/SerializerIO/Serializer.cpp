#include "pch.h"
#include "Serializer.h"

Serializer::~Serializer() = default;
Serializer::Serializer(nlohmann::json& write) noexcept : m_mode{ Mode::Write } { m_write = &write; }
Serializer::Serializer(const nlohmann::json& read) noexcept : m_mode{ Mode::Read } { m_read = &read; }

bool Serializer::IsWrite() const noexcept
{
    return m_mode == Mode::Write;
}