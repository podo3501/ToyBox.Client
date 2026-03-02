#pragma once

wstring FindRootByMarker(wstring markFilename);
wstring FindResourcePath() noexcept;
filesystem::path MakeAbsolutePath(const filesystem::path& base, const filesystem::path& input) noexcept;