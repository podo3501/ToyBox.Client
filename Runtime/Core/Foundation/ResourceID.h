#pragma once

namespace Core
{
    enum class ResourceIDType
    {
        Invalid,
        Builtin,
        Runtime,
        Path,
    };

    class ResourceID
    {
    public:
        ResourceID() = default;
        auto operator<=>(const ResourceID&) const = default;

        static ResourceID MakePath(std::string_view path);
        static ResourceID MakeRuntime(std::string_view name);
        static ResourceID MakeBuiltin(std::string_view name);

        ResourceIDType GetType() const;
        std::string_view GetValue() const;
        bool IsValid() const;

        const std::string& String() const { return m_value; }
        std::string& String() { return m_value; }
        const char* c_str() const { return m_value.c_str(); }
        bool empty() const { return m_value.empty(); }
        void clear() { m_value.clear(); }

    private:
        explicit ResourceID(const char* str) : m_value(str) {}
        explicit ResourceID(std::string str) : m_value(std::move(str)) {}

        std::string m_value;
    };
}

template<>
struct std::hash<Core::ResourceID>
{
    size_t operator()(const Core::ResourceID& id) const noexcept
    {
        return std::hash<std::string>{}(id.String());
    }
};