#include "pch.h"
#include "ComponentController.h"
#include "FloatingComponent.h"
#include "ComponentSelector.h"
#include "Toy/Locator/InputLocator.h"
#include "Toy/UserInterface/UIComponent/Traverser/UITraverser.h"
#include "Toy/UserInterface/UIComponent/Components/Panel.h"
#include "Toy/UserInterface/CommandHistory/UserInterface/UICommandHistory.h"
#include "Window/Dialog.h"
#include "Window/Utils/Common.h"

using namespace UITraverser;

ComponentController::~ComponentController() = default;
ComponentController::ComponentController(IRenderer* renderer, 	TextureResourceBinder* resBinder, 
	UIComponent* mainComponent, const string& mainWndName) noexcept :
	m_uiWindow{ nullptr },
	m_cmdHistory{ make_unique<UICommandHistory>() },
	m_floater{ make_unique<FloatingComponent>(renderer, resBinder, mainWndName)},
	m_selector{ make_unique<ComponentSelector>(renderer, resBinder, mainComponent, m_cmdHistory.get()) }
{}

void ComponentController::SetPanel(UIComponent* panel) noexcept
{
	m_selector->SetPanel(panel);
}

void ComponentController::SetUIWindow(ImGuiWindow* mainWnd) noexcept
{
	m_uiWindow = mainWnd;
	m_selector->SetUIWindow(mainWnd);
}

bool ComponentController::CheckAttachComponent(IToolInputManager* input) noexcept
{
	if (!m_uiWindow) return false;
	if (!m_floater->IsComponent()) return false;
	if (!input->IsInputAction(Keyboard::LeftShift, MouseButton::Left)) return false;

	const XMINT2& mousePosition = input->GetPosition();
	AttachSelectedComponent(m_cmdHistory.get(), m_selector.get(), m_floater.get(), mousePosition);

	return true;
}

bool ComponentController::CheckDetachComponent(IToolInputManager* input) noexcept
{
	if (m_floater->IsComponent()) return false;
	if (!input->IsInputAction(Keyboard::D, InputState::Pressed)) return false;

	auto detachComponent = DetachSelectedComponent(m_cmdHistory.get(), m_selector.get());
	if (!detachComponent)
		return true;

	//DetachµÈ Component¸¦ RenderTexture
	if (!m_floater->ComponentToFloating(move(detachComponent)))
	{
		Tool::Dialog::ShowInfoDialog(DialogType::Error, "Failed to load the resource.");
		return true;
	}

	return true;
}

bool ComponentController::CheckDeleteComponent(IToolInputManager* input) noexcept
{
	if (m_floater->IsComponent()) return false;
	if (!input->IsInputAction(Keyboard::Delete, InputState::Pressed)) return false;

	DetachSelectedComponent(m_cmdHistory.get(), m_selector.get());

	return true;
}

bool ComponentController::CheckUndoComponent(IToolInputManager* input) noexcept
{
	if (m_floater->IsComponent()) return false;
	if (!input->IsInputAction(Keyboard::LeftControl, Keyboard::Z)) return false;

	return m_cmdHistory->Undo();
}

bool ComponentController::CheckRedoComponent(IToolInputManager* input) noexcept
{
	if (m_floater->IsComponent()) return false;
	if (!input->IsInputAction(Keyboard::LeftControl, Keyboard::Y)) return false;

	return m_cmdHistory->Redo();
}

bool ComponentController::CheckCloneComponent(IToolInputManager* input) noexcept
{
	if (m_floater->IsComponent()) return false;
	if (!input->IsInputAction(Keyboard::B, InputState::Pressed)) return false;
	UIComponent* component = m_selector->GetComponent();
	if (!component) return false;

	auto clone = Clone(component);
	if(!clone)
	{
		Tool::Dialog::ShowInfoDialog(DialogType::Alert, "Cannot be cloned. Could it be a component that cannot be detached?");
		return true;
	}
	
	if (!m_floater->ComponentToFloating(move(clone)))
	{
		Tool::Dialog::ShowInfoDialog(DialogType::Error, "Failed to load the resource.");
		return true;
	}

	return true;
}

void ComponentController::SetActive(bool active) noexcept
{
	m_active = active;
}

bool ComponentController::ExecuteShortcutKeyCommands() noexcept
{
	if (!IsWindowFocus(m_uiWindow)) return false;

	auto input = ToolInputLocator::GetService();
	return CheckDetachComponent(input) ||
		CheckDeleteComponent(input) ||
		CheckCloneComponent(input) ||
		CheckUndoComponent(input) ||
		CheckRedoComponent(input) ||
		CheckAttachComponent(input);
}

bool ComponentController::Update() noexcept
{
	if (!m_active) return true;

	if(!ExecuteShortcutKeyCommands())
		m_selector->Update();	
	m_floater->Excute();

	return true;
}

void ComponentController::Render()
{
	if (!m_active) return;

	if (!Tool::Dialog::IsOpenDialog())
		m_floater->Render();
	m_selector->Render();
}
