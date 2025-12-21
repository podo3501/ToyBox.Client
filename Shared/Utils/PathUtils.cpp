#include "pch.h"
#include "PathUtils.h"

static filesystem::path GetExeDir()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(nullptr, buffer, MAX_PATH);
	return filesystem::path(buffer).parent_path();
}

static filesystem::path FindWorkspaceRootByMarkFile(
	filesystem::path path,
	const filesystem::path& markFile)
{
	if (filesystem::exists(path / markFile))
		return path;

	auto parent = path.parent_path();
	if (parent == path) // 파일 시스템 루트 도달
		return {};

	return FindWorkspaceRootByMarkFile(parent, markFile);
}

wstring FindRootByMarker(wstring markFilename)
{
	auto start = GetExeDir();
	auto workspace = FindWorkspaceRootByMarkFile(start, markFilename);
	if (workspace.empty()) return {};

	return workspace.generic_wstring() + L"/";
}