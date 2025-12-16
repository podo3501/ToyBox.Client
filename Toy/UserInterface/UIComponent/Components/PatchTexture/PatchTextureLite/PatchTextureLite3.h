#pragma once
#include "PatchTextureLite.h"

class PatchTextureLite1;
enum class DirectionType;

class PatchTextureLite3 : public PatchTextureLite
{
public:
	~PatchTextureLite3();
	PatchTextureLite3() noexcept;
	explicit PatchTextureLite3(DirectionType dirType) noexcept;

	static ComponentID GetTypeStatic() { return ComponentID::PatchTextureLite3; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual bool operator==(const UIComponent& rhs) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;
	//PatchTextureLite
	virtual bool BindSourceInfo(size_t index, const vector<Rectangle>& sources) override;

	bool Setup(const XMUINT2& size, DirectionType dirType);
	Rectangle GetSource() const noexcept;

protected:
	PatchTextureLite3(const PatchTextureLite3& other);
	virtual unique_ptr<UIComponent> CreateClone() const override;

private:
	void SetDirectionType(DirectionType dirType) noexcept;

	DirectionType m_dirType;
};