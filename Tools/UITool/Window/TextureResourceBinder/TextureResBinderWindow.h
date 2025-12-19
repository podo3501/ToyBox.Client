#pragma once
#include "../InnerWindow.h"

struct IRenderer;
struct IImguiRegistry;
struct IToolInputManager;
class RenderTexture;
class PatchTextureStd1;
class MainSourceExtractor;
class TextureResourceBinder;
class TexResCommandHistory;
class EditFontTexture;
class EditSourceTexture;

class TextureResBinderWindow : public InnerWindow
{
public:
    ~TextureResBinderWindow();
    TextureResBinderWindow(IRenderer* renderer, IImguiRegistry* imguiRegistry);
    virtual void Render(ImGuiIO* io) override;
    virtual bool SaveScene(const wstring& filename) override;
    virtual wstring GetSaveFilename() const noexcept override;
    
    bool Create(const wstring& filename = L""); //Json파일
    void SetTexture(PatchTextureStd1* pTex1) noexcept;
    void Update();
    bool IsOpen() const noexcept { return m_isOpen; }
    void Open() noexcept { m_isOpen = true; }
    ImGuiWindow* GetWindow() noexcept { return m_window; }

private:
    void CheckWindowMoved(IToolInputManager* toolInput) noexcept;
    bool CheckUndoRedo(IToolInputManager* toolInput);
    ImVec2 GetWindowSize() const noexcept;
    void RenderResourceWindow();

    IRenderer* m_renderer{ nullptr };
    IImguiRegistry* m_imguiRegistry{ nullptr };
    ImGuiWindow* m_window{ nullptr };
    unique_ptr<TextureResourceBinder> m_resBinder;
    unique_ptr<TexResCommandHistory> m_cmdHistory;
    unique_ptr<RenderTexture> m_renderTex; //InnerWindow를 그리는 텍스쳐
    PatchTextureStd1* m_sourceTexture; //작업할 텍스쳐
    unique_ptr<EditFontTexture> m_editFontTexture;
    unique_ptr<EditSourceTexture> m_editSourceTexture;
    ImVec2 m_windowPosition{};
    ImVec2 m_size;
    bool m_isOpen{ false };
};

