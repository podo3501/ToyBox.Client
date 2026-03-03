#pragma once
#include <string>
#include "Platform/Serializer/Serializer.h"
#include "Platform/Resource/IResourceManager.h"

class JsonObjectIO
{
public:
	template<HasSerialize T>
	static bool Write(T& obj, const filesystem::path& filename, IResourceManager* resManager);
	template<HasSerialize T>
	static bool Read(T& obj, const filesystem::path& filename, IResourceManager* resManager);
};

template<HasSerialize T>
bool JsonObjectIO::Write(T& obj, const filesystem::path& filename, IResourceManager* resManager)
{
	nlohmann::json wData;
	SerializeClass(wData, obj);

	const string text = wData.dump(4);
	return resManager->WriteText(filename, text);
}

template<HasSerialize T>
bool JsonObjectIO::Read(T& obj, const filesystem::path& filename, IResourceManager* resManager)
{
	string text;
	ReturnIfFalse(resManager->ReadText(filename, text));

	nlohmann::json rData = nlohmann::json::parse(text);
	DeserializeClass(rData, obj);

	return true;
}