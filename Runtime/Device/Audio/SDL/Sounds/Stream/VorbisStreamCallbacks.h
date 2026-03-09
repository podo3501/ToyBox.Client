#pragma once
#include "ogg/ogg.h"
#include "vorbis/vorbisfile.h"

namespace Vorbis
{
	ov_callbacks CreateCallbacks();
	size_t ReadFunc(void* ptr, size_t size, size_t nmemb, void* datasource);
	int SeekFunc(void* datasource, ogg_int64_t offset, int whence);
	long TellFunc(void* datasource);
	int CloseFunc(void*);
}


