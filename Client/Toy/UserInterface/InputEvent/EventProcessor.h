#pragma once
#include "Shared/Foundation/NoCopyNoMove.h"

struct IAudioManager;
class SoundTheme;
class MouseEventReceiver;
enum class InputResult;
enum class InteractState;
class EventProcessor : private NoCopyNoMove
{
public:
	~EventProcessor();
	explicit EventProcessor(IAudioManager* audioManager);
	InputResult ProcessHover(MouseEventReceiver* eventReceiver) noexcept;

private:
	bool PlaySound(InteractState interactState, MouseEventReceiver* eventReceiver);

	IAudioManager* m_audioManager{ nullptr };
	unique_ptr<SoundTheme> m_theme;
};