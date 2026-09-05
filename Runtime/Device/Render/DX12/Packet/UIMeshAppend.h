#pragma once

struct DrawUIItem;
struct UIBatchBuffer;

// ViewDrawList::ui의 각 아이템을 CPU에서 world/uv를 구워 단일 batch buffer에 append.
// static mesh(quad 등)의 CPU 템플릿(StaticMeshResource::GetCPUTemplate)이 있는 아이템만 처리한다.
void AppendUIItems(
    std::span<const DrawUIItem> uiItems,
    UIBatchBuffer& buffer);