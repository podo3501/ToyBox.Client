#pragma once

struct IResourceManager;
struct IResourceManager;
class FileTab;
class Serializer;
class RecentFiles
{
    static constexpr size_t MaxRecentFiles = 20;    //More 와 합친 총 갯수
    static constexpr size_t MaxShownFiles = 7;    // "Open Recent" 메뉴에 표시할 파일 개수
    static constexpr const wchar_t* RecentFilename = L"Tool/OpenRecentFiles.json";

public:
    ~RecentFiles();
    RecentFiles(IResourceManager* resManager);

    void AddFile(const wstring& filename);
    bool OpenFile(FileTab& menuBar);
    bool Show();
    void Serialize(Serializer& serializer);

private:
    void ShowMoreMenu();

    IResourceManager* m_resManager{ nullptr };
    deque<wstring> m_recentFiles;
    wstring m_file;
};
