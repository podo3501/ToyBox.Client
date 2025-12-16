#pragma once
#include <memory>
#include "Shared/Framework/Locator.h"
#include "Shared/SerializerIO/Storage/IJsonStorage.h"
#include "Shared/SerializerIO/Storage/FileStorage.h"
#include "Shared/SerializerIO/Storage/MemoryStorage.h"

using JsonStorageLocator = Locator<IJsonStorage>;
enum class StorageType { File, Memory };

inline IJsonStorage* GetJsonStorage() noexcept { return JsonStorageLocator::GetService(); }

[[nodiscard]]
inline std::unique_ptr<IJsonStorage> InitializeJsonStorage(StorageType storageType) //어디에 저장될지 지정
{
	std::unique_ptr<IJsonStorage> storage;
	switch (storageType)
	{
	case StorageType::File: storage = std::make_unique<FileStorage>(); break;
	case StorageType::Memory: storage = std::make_unique<MemoryStorage>(); break;
	}
	JsonStorageLocator::Provide(storage.get());
	return move(storage);
}