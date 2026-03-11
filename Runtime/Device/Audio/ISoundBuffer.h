#pragma once

class ISoundBuffer
{
public:
    enum class BufferType
    {
        Static,
        Stream
    };

	virtual ~ISoundBuffer() = default;
    virtual BufferType GetType() const = 0;
};

class IStaticSoundBuffer : public ISoundBuffer
{
public:
    virtual bool LoadFromMemory(Core::ByteBuffer fileBuffer) = 0;
};
