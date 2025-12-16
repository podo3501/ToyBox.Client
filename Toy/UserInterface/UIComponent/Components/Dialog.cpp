#include "pch.h"
#include "Dialog.h"
#include "Shared/SerializerIO/SerializerIO.h"

Dialog::~Dialog() = default;
Dialog::Dialog()
{};

Dialog::Dialog(const Dialog& other) :
	UIComponent{ other }
{}

unique_ptr<UIComponent> Dialog::CreateClone() const
{
	return unique_ptr<Dialog>(new Dialog(*this)); //복사생성자를 protected로 만들려면 make_unique대신 new를 사용해야한다.
}
