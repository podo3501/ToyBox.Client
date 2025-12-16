#pragma once

struct IRenderer;
class TextureResourceBinder;
class EditWindow;
class UIComponent;
class UICommandHistory;
enum class ComponentID;

class EditWindowFactory 
{
    using FactoryFunction = function<unique_ptr<EditWindow>(UIComponent*, UICommandHistory*, TextureResourceBinder*)>;

public:
    static void RegisterFactories();
    static unique_ptr<EditWindow> CreateEditWindow(UIComponent* component, UICommandHistory* cmdHistory,
        TextureResourceBinder* resBinder);

private:
    static inline void Register(ComponentID id, FactoryFunction factory) noexcept { (*m_factoryMap)[id] = factory; }

    template <typename EditType, typename ComponentType, typename... Args>
    static unique_ptr<EditWindow> CreateEdit(UIComponent* component, Args&&... args)
    {
        return make_unique<EditType>(ComponentCast<ComponentType>(component), forward<Args>(args)...);
    }

    template <typename EditType, typename ComponentType>
    static void RegisterEditWindow(ComponentID id)
    {
        Register(id, [](UIComponent* component, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) {
            return CreateEdit<EditType, ComponentType>(component, cmdHistory);
            });
    }

    template <typename EditType, typename ComponentType>
    static void RegisterEditWindowWithBinder(ComponentID id)
    {
        Register(id, [](UIComponent* component, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) {
            return CreateEdit<EditType, ComponentType>(component, cmdHistory, resBinder);
            });
    }

    static optional<unordered_map<ComponentID, FactoryFunction>> m_factoryMap;
};