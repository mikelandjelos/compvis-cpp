#include "examples/registry.h"

#include <algorithm>

namespace examples
{

static std::vector<Item>& registry()
{
    static std::vector<Item> items;
    return items;
}

void register_example(const Item& item)
{
    auto& r = registry();
    // Avoid duplicate names
    auto it = std::find_if(r.begin(), r.end(), [&](const Item& x)
                           { return std::string_view{x.name} == std::string_view{item.name}; });
    if (it == r.end())
        r.push_back(item);
}

const std::vector<Item>& all()
{
    return registry();
}

const Item* find(std::string_view name)
{
    const auto& r = registry();
    for (const auto& it : r)
    {
        if (std::string_view{it.name} == name)
            return &it;
    }
    return nullptr;
}

Registrar::Registrar(Item item)
{
    register_example(item);
}

} // namespace examples
