#pragma once
#include "../UIComponent.h"

class SerializerIO;

class Dialog : public UIComponent
{
protected:
	Dialog(const Dialog& o);

	static ComponentID GetTypeStatic() { return ComponentID::Dialog; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual unique_ptr<UIComponent> CreateClone() const override;

public:
	Dialog();
	~Dialog();
	
private:
};