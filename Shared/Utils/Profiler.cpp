#include "pch.h"
#include "Profiler.h"
#include <chrono>
#include <thread>

//Startup 및 Shutdown을 해 주는 이유는 tdd할때 속도문제 때문이다. tdd가 느리면 테스트를 귀찮게 여길 수 있기 때문이다.
void TracyStartupProfiler()
{
#ifdef TRACY_ENABLE
	tracy::StartupProfiler();

	// 연결 대기 시간(연결이 안정될 때까지)
	constexpr auto connectionTimeout = chrono::milliseconds(1000); // 1초 타임아웃
	auto startTime = chrono::steady_clock::now();

	while (!TracyIsConnected)
	{
		this_thread::sleep_for(std::chrono::milliseconds(10));
		if (std::chrono::steady_clock::now() - startTime > connectionTimeout) // 타임아웃을 추가
			break;
	}
#endif
}

void TracyShutdownProfiler()
{
#ifdef TRACY_ENABLE
	FrameMark; // 마지막 프레임 구분
	this_thread::sleep_for(chrono::milliseconds(100)); // 데이터 전송 시간 확보

	tracy::ShutdownProfiler(); // 내부 쓰레드 및 버퍼 정리
#endif
}