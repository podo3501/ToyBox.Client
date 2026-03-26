#pragma once

struct InputState
{
    bool previous{ false };
    bool current{ false };

    void Update(bool isDown)
    {
        previous = current;
        current = isDown;
    }

    bool IsPressed() const { return current && !previous; }
    bool IsReleased() const { return !current && previous; }
    bool IsHeld() const { return current; }
    bool IsUp() const { return !current; }
};