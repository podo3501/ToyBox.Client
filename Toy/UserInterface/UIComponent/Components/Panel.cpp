#include "pch.h"
#include "Panel.h"
#include "IRenderer.h"
#include "Shared/Serializer/Serializer.h"

Panel::~Panel() = default;
Panel::Panel()
{}

Panel::Panel(const Panel& other) :
    UIComponent{ other }
{}

Panel::Panel(const string& name, const UILayout& layout) noexcept :
    UIComponent(name, layout)
{}

unique_ptr<UIComponent> Panel::CreateClone() const
{
    return unique_ptr<Panel>(new Panel(*this));
}

bool Panel::Setup(const UILayout& layout)
{
    SetLayout(layout);
    return true;
}

void Panel::Serialize(Serializer& serializer)
{
    UIComponent::Serialize(serializer);
}