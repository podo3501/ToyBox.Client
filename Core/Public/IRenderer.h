#pragma once

struct IComponent;
struct ITextureBinder;
//로딩할때 사용하는 인터페이스

struct ITextureLoad
{
public:
    virtual ~ITextureLoad() {};

    virtual bool LoadTexture(const wstring& filename, size_t& outIndex, XMUINT2* outSize, UINT64* outGfxMemOffset) = 0;
    virtual bool LoadFont(const wstring& filename, size_t& outIndex) = 0;
    virtual void ReleaseTexture(size_t idx) noexcept = 0;
    virtual void AddRef(size_t index) noexcept = 0; //Clone했을때 refcount를 맞추어 주기위해 사용하는 함수인데, TextureResourceBinder는 Clone할 필요가 없기 때문에 잘 안쓰일 가능성이 있다.
};

struct ITextureRender
{
public:
    virtual ~ITextureRender() {};

    virtual void Render(size_t index, const RECT& dest, const RECT* source) = 0;
    virtual void DrawString(size_t index, const wstring& text, const Vector2& pos, const FXMVECTOR& color) const = 0;
};

//데이터를 얻어올때 사용하는 인터페이스
struct ITextureController
{
public:
    virtual ~ITextureController() {};

    //Text관련
    virtual Rectangle MeasureText(size_t index, const wstring& text, const Vector2& position) = 0;
    virtual float GetLineSpacing(size_t index) const noexcept = 0;

    //Texture
    virtual void SetTextureRenderer(function<void(size_t index, ITextureRender*)> rendererFn) noexcept = 0;
    virtual bool CreateRenderTexture(const Rectangle& targetRect, size_t& outIndex, UINT64* outGfxMemOffset) = 0;
    virtual optional<vector<Rectangle>> GetTextureAreaList(size_t index, const UINT32& bgColor) = 0;
    virtual void ModifyRenderTexturePosition(size_t index, const XMINT2& leftTop) noexcept = 0;
    virtual bool ModifyRenderTextureSize(size_t index, const XMUINT2& size) = 0;
    virtual void ReleaseTexture(size_t idx) noexcept = 0; 
    virtual void AddRef(size_t index) noexcept = 0; //현재는 쓰이지 않지만, 리소스를 공유해야할때 쓰는 함수이다.
};

//Imgui ui 만들때 사용
struct ImGuiIO;
struct IImguiComponent
{
public:
    virtual ~IImguiComponent() {};

    virtual void Render(ImGuiIO* io) = 0;
    //virtual void Update() = 0;
};

struct IRenderer
{
public:
    virtual ~IRenderer() {};

    virtual bool Initialize() = 0;

    virtual void SetComponentRenderer(function<void(ITextureRender*)> rendererFn) noexcept = 0;
    virtual void AddImguiComponent(IImguiComponent* item) = 0;
    virtual void RemoveImguiComponent(IImguiComponent* comp) noexcept = 0;

    virtual bool LoadTextureBinder(ITextureBinder* textureBinder) = 0;

    virtual void Draw() = 0;
    virtual ITextureController* GetTextureController() const noexcept = 0;
    
    virtual void OnActivated() = 0;
    virtual void OnDeactivated() = 0;
    virtual void OnSuspending() = 0;
    virtual void OnResuming() = 0;
    virtual void OnWindowMoved() = 0;
    virtual void OnDisplayChange() = 0;
    virtual void OnWindowSizeChanged(int width, int height) = 0;
};

std::unique_ptr<IRenderer> CreateRenderer(HWND hwnd, int width, int height, bool bUseImgui);