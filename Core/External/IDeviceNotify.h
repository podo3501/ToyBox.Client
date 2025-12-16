#pragma once

// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
namespace DX
{
    interface IDeviceNotify
    {
        virtual void OnDeviceLost() = 0;
        virtual void OnDeviceRestored() = 0;

    protected:
        ~IDeviceNotify() = default;
    };
}
