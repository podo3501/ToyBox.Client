#include "pch.h"
#include "Core/Utils/PathUtils.h"

class ConsoleLoop
{
public:
	explicit ConsoleLoop(const wstring& resPath) noexcept;

private:
	wstring m_resPath;
};

ConsoleLoop::ConsoleLoop(const wstring& resPath) noexcept :
	m_resPath{ resPath }
{}

static unique_ptr<ConsoleLoop> CreateConsoleAppLoop()
{
	return make_unique<ConsoleLoop>(FindResourcePath());
}

int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto consoleAppLoop = CreateConsoleAppLoop();

	return 0;
}