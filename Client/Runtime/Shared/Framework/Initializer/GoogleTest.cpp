#include "pch.h"
#include "GoogleTest.h"

int CreateGoogleTest(int argc, char** argv, const wstring& projectName, ::testing::Environment* env)
{
	//쓰레드가 돌아갈때 강제로 종료할 경우 프로그램이 살아있는 경우가 있어서 
	//다음 테스트에 오동작 하는 경우가 있다. 그럴때를 대비해서 프로그램이 띄워져 있는지 체크
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, false, projectName.c_str());
	if (!mutex)
		mutex = CreateMutex(nullptr, true, projectName.c_str());
	else {
		OutputDebugString(L"\nTest 실패! 프로그램이 중복 실행 되었습니다. 자동 종료 합니다.\n\n");
		return 0;
	}

	::testing::GTEST_FLAG(color) = "yes"; //yes : 컬러 사용, no : 컬러 사용 x, auto : 자동
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(env);

	auto silent_run = false;
	if (silent_run)
	{
		auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
		delete listeners.Release(listeners.default_result_printer());
	}
	
	return RUN_ALL_TESTS();
}