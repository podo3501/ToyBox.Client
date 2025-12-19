#include "pch.h"
#include "UserInterfaceWindow.h"
#include "Window/Utils/Common.h"
#include "ComponentController.h"
#include "Toy/Locator/InputLocator.h"
#include "Toy/UserInterface/UIComponent/Traverser/UITraverser.h"
#include "Toy/UserInterface/UIComponent/Traverser/UIDetailTraverser.h"
#include "Toy/UserInterface/UIComponentLocator.h"
#include "Toy/UserInterface/UIComponent/Components/RenderTexture.h"
#include "Toy/UserInterface/UIComponent/Components/UIModuleAsComponent.h"
#include "Toy/UserInterface/UIComponent/Components/Panel.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "Toy/UserInterface/UIModule.h"
#include "Core/Public/IImguiRegistry.h"
#include "Shared/System/StepTimer.h"

using namespace UITraverser;
using namespace UIDetailTraverser;

int UserInterfaceWindow::m_uiWindowIndex = 0;
UserInterfaceWindow::~UserInterfaceWindow()
{
	InputLocator::Provide(m_inputManager); //null input으로 종료되면 다음에 창을 열때 문제가 된다.
	ReleaseUIModule(GetName());
	m_imguiRegistry->RemoveComponent(this);
}

UserInterfaceWindow::UserInterfaceWindow(IRenderer* renderer, IImguiRegistry* imguiRegistry) :
	InnerWindow{ "UserInterface Window " + to_string(m_uiWindowIndex++) },
	m_renderer{ renderer },
	m_imguiRegistry{ imguiRegistry }
{
	//inputManager를 nullInputManager로 대체해서 마우스가 client 코드만 반응하지 않도록 한다.
	//툴의 마우스는 ToolInputManager를 통해서 반응하게 된다. 
	m_inputManager = InputLocator::GetService();
	m_nullInputManager = CreateNullInputManager();
	InputLocator::Provide(m_nullInputManager.get());

	m_imguiRegistry->AddComponent(this);
}

ImVec2 UserInterfaceWindow::GetPanelSize() const noexcept
{
	Panel* main = GetUIModule()->GetMainPanel();
	return XMUINT2ToImVec2(main->GetSize());
}

bool UserInterfaceWindow::SetupProperty(UIModule* uiModule)
{
	m_controller = make_unique<ComponentController>(m_renderer, uiModule->GetTexResBinder(),
		uiModule->GetMainPanel(), GetName());

	unique_ptr<UIModuleAsComponent> asComponent = CreateComponent<UIModuleAsComponent>(uiModule);
	m_mainRenderTexture = CreateComponent<RenderTexture>(move(asComponent));
	ReturnIfFalse(BindTextureSourceInfo(m_mainRenderTexture.get(), nullptr)); //모듈안에 resBinder가 있기 때문에 이것은 nullptr로 한다.

	ToggleToolMode();
	m_isOpen = true;

	return true;
}

bool UserInterfaceWindow::CreateScene(const XMUINT2& size)
{
	auto texResBinder = CreateTextureResourceBinder(L"UI/SampleTexture/SampleTextureBinder.json", m_renderer);
	UIModule* module = CreateUIModule(GetName(), UILayout(size, Origin::LeftTop), "Main", move(texResBinder));
	return SetupProperty(module);
}

bool UserInterfaceWindow::CreateScene(const wstring& filename)
{
	auto texResBinder = CreateTextureResourceBinder(L"UI/SampleTexture/SampleTextureBinder.json", m_renderer);
	UIModule* module = CreateUIModule(GetName(), filename, move(texResBinder));
	return SetupProperty(module);
}

bool UserInterfaceWindow::SaveScene(const wstring& filename)
{
	UIModule* module = GetUIModule();
	return module->Write(filename);
}

wstring UserInterfaceWindow::GetSaveFilename() const noexcept
{
	UIModule* module = GetUIModule();
	return module->GetFilename();
}

void UserInterfaceWindow::ChangeWindowSize(const ImVec2& size)
{
	const XMUINT2& uint2Size = ImVec2ToXMUINT2(size);
	ChangeSize(m_mainRenderTexture.get(), uint2Size);
}

UIModule* UserInterfaceWindow::GetUIModule() const noexcept
{
	UIComponent* component = m_mainRenderTexture->GetRenderedComponent();
	UIModuleAsComponent* asComponent = ComponentCast<UIModuleAsComponent*>(component);
	return asComponent->GetUIModule();
}

void UserInterfaceWindow::ToggleToolMode() noexcept
{
	m_isTool = !m_isTool;
	GetUIModule()->EnableToolMode(m_isTool);
	m_controller->SetActive(m_isTool);

	if(m_isTool)
		InputLocator::Provide(m_nullInputManager.get());
	else
		InputLocator::Provide(m_inputManager);
}

void UserInterfaceWindow::CheckActiveUpdate(IToolInputManager* toolInput) noexcept
{
	if (!toolInput->IsInputAction(Keyboard::F5, InputState::Pressed)) return;
	
	ToggleToolMode();
}

void UserInterfaceWindow::CheckWindowResized(IToolInputManager* toolInput)
{
	static ImVec2 startSize{};
	if (toolInput->IsInputAction(MouseButton::Left, InputState::Pressed))
		startSize = m_window->Size;

	if (!toolInput->IsInputAction(MouseButton::Left, InputState::Released))
		return;
	
	if(startSize != m_window->Size && !m_window->Collapsed)
	{
		ImVec2 newWndSize{ m_window->Size.x, m_window->Size.y - GetFrameHeight()};
		ChangeWindowSize(newWndSize);
	}
}

void UserInterfaceWindow::CheckWindowMoved(IToolInputManager* toolInput) noexcept
{	
	if (m_windowPosition == m_window->Pos)
		return;
	
	SetMouseStartOffset(toolInput, m_window);
	SetMouseStartOffset(m_inputManager, m_window);

	m_windowPosition = m_window->Pos;
}

void UserInterfaceWindow::Update(const DX::StepTimer& timer)
{
	if (!m_window) return;
	
	auto toolInput = ToolInputLocator::GetService();
	CheckWindowMoved(toolInput); //?!? Check라는 이름을 쓰니까 좀 어색하다 bool 값을 리턴하는 이름이라.
	CheckWindowResized(toolInput); //창이 변했을때 RenderTexture를 다시 만들어준다.
	CheckActiveUpdate(toolInput);
		
	m_controller->Update();
	UIDetailTraverser::Update(m_mainRenderTexture.get(), timer);

	if (float elapsedTime = static_cast<float>(timer.GetElapsedSeconds()); elapsedTime)
		m_fps = 1.0f / elapsedTime;
}

void UserInterfaceWindow::Render(ImGuiIO* io)
{
	if (!m_isOpen)
		return;

	SetupImGuiWindow();
	SetupWindowAppearing();
	HandleMouseEvents();
	RenderContent();

	ImGui::End();
}

void UserInterfaceWindow::SetupWindowAppearing() noexcept
{
	if (!ImGui::IsWindowAppearing()) return;
	
	m_window = GetImGuiWindow();
	m_controller->SetUIWindow(m_window);

	CheckWindowMoved(ToolInputLocator::GetService());
}

void UserInterfaceWindow::HandleMouseEvents()
{
	IgnoreMouseClick(m_window);
	if (m_window && IsWindowFocus(m_window))
		Tool::MouseCursor::Render();
}

void UserInterfaceWindow::ShowStatusBar() const//상태 표시줄(임시)
{
	if (m_isTool)
	{
		ImGui::SetCursorPos({ 0, GetFrameHeight() });
		ImGui::Text("Frames per Second: %.4f", m_fps);
		ImGui::SetCursorPos(ImVec2(0, ImGui::GetWindowHeight() - ImGui::GetFontSize()));
		ImGui::Text("Right Mouse Button: Floating Menu     Shift + Left Mouse Button: Attach     D: Detach     B: Clone     Del: Delete     F5: Update State     Ctrl + Z, Y: Undo/Redo");
	}
	else
	{
		ImGui::SetCursorPos({ 0, GetFrameHeight() });
		ImGui::Text("Frames per Second: %.4f  --Status update has been activated--", m_fps);
	}	
}

void UserInterfaceWindow::SetupImGuiWindow()
{
	const auto& panelSize = GetPanelSize();
	auto windowSize = ImVec2(panelSize.x, panelSize.y + GetFrameHeight());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(windowSize);
	ImGui::Begin(GetName().c_str(), &m_isOpen, ImGuiWindowFlags_None);
	ImGui::PopStyleVar();
}

void UserInterfaceWindow::RenderContent()
{
	ImGui::Image(m_mainRenderTexture->GetGraphicMemoryOffset(), GetPanelSize());
	m_controller->Render();
	ShowStatusBar();
}
