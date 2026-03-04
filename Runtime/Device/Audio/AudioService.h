#pragma once
#include "SoundTable.h"

struct IAudioBackend;
struct IResourceManager;
class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	AudioService(SoundTable sndTable, 
		unique_ptr<IAudioBackend> audioBackend,
		IResourceManager* resManager) noexcept;

	bool LoadSound(string_view index);

private:
	SoundTable m_sndTable;
	unique_ptr<IAudioBackend> m_audioBackend;
	IResourceManager* m_resManager{ nullptr };
};