#include "pch.h"
#include "Resource.h"

Resource::~Resource() = default;
Resource::Resource(ComPtr<ID3D12Resource> resource) : 
	m_resource(std::move(resource))
{}

