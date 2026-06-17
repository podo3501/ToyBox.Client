#include "pch.h"
#include "ResourceProviders.h"

ResourceProviders::~ResourceProviders() = default;
ResourceProviders::ResourceProviders(Device& device) :
	m_device{ device }
{}