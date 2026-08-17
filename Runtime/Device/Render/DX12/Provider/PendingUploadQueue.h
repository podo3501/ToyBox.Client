#pragma once
#include <queue>
#include <vector>
#include <cstddef>
#include <algorithm>

// 이번 아이템을 더했을 때 budget을 넘는지 미리 체크해서, budget 이내로 최대한 타이트하게 채운다.
// 단, batch가 비어있는 상태(첫 아이템)라면 budget을 넘더라도 무조건 담아서
// budget보다 큰 아이템이 영원히 대기열에 남는 걸 방지한다.

template <typename RequestT>
class PendingUploadQueue
{
public:
    void Push(RequestT req) { m_pending.push(std::move(req)); }

    template <typename FlushFn>
    void Flush(size_t budgetBytes, FlushFn&& flushFn, size_t reserveHint = 32)
    {
        size_t usedBytes = 0;
        std::vector<RequestT> batch;
        batch.reserve(std::min(m_pending.size(), reserveHint));

        while (!m_pending.empty())
        {
            RequestT& req = m_pending.front();

            if (usedBytes + req.estimatedBytes > budgetBytes && !batch.empty())
                break;

            usedBytes += req.estimatedBytes;
            batch.push_back(std::move(req));
            m_pending.pop();
        }

        if (batch.empty())
            return;

        flushFn(batch);
    }

private:
    std::queue<RequestT> m_pending;
};