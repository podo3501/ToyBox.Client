#pragma once

struct IAudioManager;
class SoundTheme;
class MouseEventReceiver;
enum class InputResult;
enum class InteractState;
class EventProcessor : private NoCopyNoMove
{
public:
	~EventProcessor();
	//explicit EventProcessor(IAudioManager* audioManager);
	EventProcessor();
	InputResult ProcessHover(MouseEventReceiver* eventReceiver) noexcept;

private:
	bool PlaySound(InteractState interactState, MouseEventReceiver* eventReceiver);

	unique_ptr<SoundTheme> m_theme;
};