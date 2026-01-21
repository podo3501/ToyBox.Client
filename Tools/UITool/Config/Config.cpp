#include "pch.h"
#include "Config.h"
#include "Shared/Data/JsonObjectIO.h"

using namespace Tool;

static constexpr const wchar_t* ResolutionFilename = L"Tool/Config.json";
unique_ptr<IJsonStorage> Config::m_storage = nullptr;
ResolutionType Config::m_resolutionType = ResolutionType::R800x600;
Config* Config::m_Instance = nullptr;

Config::Config()
{
	m_Instance = this;
	JsonStorageDesc desc;
	desc.SetFilename<StorageKey::Config>(ResolutionFilename);
	m_storage = CreateJsonStorage(desc);
	JsonObjectIO::Read<StorageKey::Config>(*this, m_storage.get());
}

Config::~Config()
{}

void Config::SetResolution(ResolutionType type) noexcept
{
	if (m_resolutionType == type)
		return;

	m_resolutionType = type;
	JsonObjectIO::Write<StorageKey::Config>(*m_Instance, m_storage.get());
}

ResolutionType Config::GetResolution() noexcept
{
	return m_resolutionType;
}

XMUINT2 Config::GetResolutionInCoordinate() noexcept
{
	switch (m_resolutionType)
	{
	case ResolutionType::R2560x1440: return XMUINT2{ 2560, 1440 };
	case ResolutionType::R1920x1080: return XMUINT2{ 1920, 1080 };
	case ResolutionType::R1600x900: return XMUINT2{ 1600, 900 };
	case ResolutionType::R1280x720: return XMUINT2{ 1280, 720 };
	case ResolutionType::R1024x768: return XMUINT2{ 1024, 768 };
	case ResolutionType::R800x600: return XMUINT2{ 800, 600 };
	}

	return XMUINT2{ 800, 600 };
}

void Config::Serialize(Serializer& serializer)
{
	serializer.Process("Resolution", m_resolutionType);
}