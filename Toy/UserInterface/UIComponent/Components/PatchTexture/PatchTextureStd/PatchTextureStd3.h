#pragma once
#include "PatchTextureStd.h"

struct TextureSourceInfo;
class PatchTextureStd1;
enum class DirectionType;

class PatchTextureStd3 : public PatchTextureStd
{
public:
	~PatchTextureStd3();
	PatchTextureStd3() noexcept;

	static ComponentID GetTypeStatic() { return ComponentID::PatchTextureStd3; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual bool operator==(const UIComponent& rhs) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;

	bool Setup(const UILayout& layout, DirectionType dirType, const string& bindKey, size_t sourceIndex = 0) noexcept;
	inline bool Setup(DirectionType dirType, const string& bindKey, size_t sourceIndex = 0) noexcept { return Setup({}, dirType, bindKey, sourceIndex); }
	Rectangle GetSource() const noexcept;
	inline DirectionType GetDirectionType() const noexcept { return m_dirType; }

protected:
	PatchTextureStd3(const PatchTextureStd3& o);
	virtual unique_ptr<UIComponent> CreateClone() const override;

private:
	void SetDirectionType(DirectionType dirType) noexcept;

	DirectionType m_dirType;
};