#include "pch.h"
#include "IJsonStorage.h"
#include "Storage/FileStorage.h"
#include "Storage/MemoryStorage.h"

unique_ptr<IJsonStorage> CreateJsonStorage(StorageType storageType)
{
	unique_ptr<IJsonStorage> storage;
	switch (storageType)
	{
	case StorageType::File: storage = make_unique<FileStorage>(); break;
	case StorageType::Memory: storage = make_unique<MemoryStorage>(); break;
	}
	return move(storage);
}
