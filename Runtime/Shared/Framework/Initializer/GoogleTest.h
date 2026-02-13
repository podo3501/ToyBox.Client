#pragma once

// 테스트 프로젝트에서 테스트를 실행시키지 않게 한다. mock을 안 쓴 테스트는 느리기 때문에 tdd에 방해가
// 되기 때문이다. 
//#define DISABLE_ToyExpr
//#define DISABLE_ToyUnit

int CreateGoogleTest(int argc, char** argv, const wstring& projectName, ::testing::Environment* env);