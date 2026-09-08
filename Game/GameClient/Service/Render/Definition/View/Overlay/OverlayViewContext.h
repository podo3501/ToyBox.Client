#pragma once
#include "../ViewTargetInfo.h"

struct OverlayViewContext
{
    explicit OverlayViewContext(ViewID id) : target{ id } {}
    ViewTargetInfo target;
};