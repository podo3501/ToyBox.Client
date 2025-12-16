#include "pch.h"
#include "RecentFiles.h"
#include "FileTab.h"
#include "Shared/Utils/StringExt.h"
#include "Shared/SerializerIO/SerializerIO.h"

RecentFiles::RecentFiles()
{
    SerializerIO::ReadJsonFromFile(RecentFilename, *this);
}

void RecentFiles::AddFile(const wstring& filename)
{
    auto it = ranges::find(m_recentFiles, filename);
    if (it != m_recentFiles.end())
        m_recentFiles.erase(it);

    m_recentFiles.push_front(filename);
    if (m_recentFiles.size() > MaxRecentFiles)
        m_recentFiles.pop_back();

    SerializerIO::WriteJsonToFile(*this, RecentFilename);
}

bool RecentFiles::OpenFile(FileTab& menuBar)
{
    auto result = menuBar.CreateUIWindowFromFile(m_file);
    if(result)
        AddFile(m_file);

    m_file.clear();    
    return result;
}

void RecentFiles::ProcessIO(SerializerIO& serializer)
{
    serializer.Process("RecentFiles", m_recentFiles);
}

bool RecentFiles::Show()
{
    if (!ImGui::BeginMenu("Open Recent"))
        return false;

    size_t shownCount = 0;
    for (const auto& file : m_recentFiles)
    {
        if (shownCount >= MaxShownFiles)
            break;

        if (file.empty())
            continue;

        if (ImGui::MenuItem(WStringToString(file).c_str()))
            m_file = file;
        ++shownCount;
    }

    ShowMoreMenu();
    ImGui::EndMenu();

    if (!m_file.empty())
        return true;

    return false;
}

void RecentFiles::ShowMoreMenu()
{
    if (m_recentFiles.size() <= MaxShownFiles)
        return;

    if (!ImGui::BeginMenu("More.."))
        return;

    for (size_t i = MaxShownFiles; i < m_recentFiles.size(); ++i)
    {
        if (ImGui::MenuItem(WStringToString(m_recentFiles[i]).c_str()))
            m_file = m_recentFiles[i];
    }
    ImGui::EndMenu();
}