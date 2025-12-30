#include "pch.h"
#include "EventProcessor.h"
#include "SoundTheme.h"
#include "MouseEventReceiver.h"
#include "Shared/System/Public/IAudioManager.h"

EventProcessor::~EventProcessor() = default;
EventProcessor::EventProcessor(IAudioManager* audioManager) :
	m_audioManager{ audioManager }
{
	m_theme = make_unique<SoundTheme>();
	m_theme->SetSoundID(0, "UI_Hover_Default");
}

InputResult EventProcessor::ProcessHover(MouseEventReceiver* eventReceiver) noexcept
{
	PlaySound(InteractState::Hovered, eventReceiver);
	return eventReceiver->OnHover();
}

bool EventProcessor::PlaySound(InteractState interactState, MouseEventReceiver* eventReceiver)
{
	const auto& sounds = eventReceiver->GetInteractSounds();
	auto it = sounds.find(interactState);
	if (it == sounds.end()) return false;

	auto soundID = m_theme->GetSoundID(it->second);
	if (soundID.empty()) return false;

	m_audioManager->Play(soundID);
	return true;
}