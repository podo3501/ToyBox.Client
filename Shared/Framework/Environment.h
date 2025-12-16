#pragma once
//App을 띄울때 필요한 최소한의 정보를 저장한다. 리소스나 해상도 같은.
class Environment
{
public:
	Environment() = delete;
	Environment(const wstring& resourcePath, const Vector2& resolution);

	const wstring& GetResourcePath() const noexcept { return m_resourcePathW; }
	const wstring& GetResourceFontPath() const noexcept { return m_fontPath; }
	const Vector2& GetResolution() const noexcept { return m_resolution; }
	Rectangle GetRectResolution() const noexcept;
	wstring GetResourceFullFilenameW(const wstring& filename) const noexcept;
	string GetResourceFullFilename(const string& filename) const noexcept;
	wstring GetRelativePath(const wstring& fullPath) const noexcept;

private:
	wstring m_fontPath;
	wstring m_resourcePathW;
	string m_resourcePath;
	Vector2 m_resolution{};
};