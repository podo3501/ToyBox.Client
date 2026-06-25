#pragma once
#include "RGTypes.h"
#include <vector>
#include <cstdint>

struct PassNodeV
{
    PassIndex index;
    std::vector<PassIndex> dependencies; // 내 앞에 실행되어야 하는 패스들 (정방향)
    std::vector<PassIndex> dependents; // 내 뒤에 실행되어야 하는 패스들 (역방향) 나중에 barrier를 만들고 나서 다시 역방향을 만든다. barrier를 만들때 역방향을 연결해 줄 수도 있지만, 다시 만들어도 비교적 비용이 싸고 유지보수가 더 쉽기 때문이다.
    int indegree{ 0 };
};

struct ResourceStateTrackerV
{
    D3D12_RESOURCE_STATES state{ D3D12_RESOURCE_STATE_COMMON };
    PassIndex lastUpdatedPass{ -1 };
};


