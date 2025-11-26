#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace examples
{

// All examples have this signature
using ExampleFn = int(int argc, char** argv);

struct Item
{
    const char* name; // unique id (e.g., "show")
    ExampleFn* fn;    // function to run
    const char* help; // short description
};

// Register a new example (called by static registrars)
void register_example(const Item& item);

// Access all registered examples
const std::vector<Item>& all();

// Find by name (returns nullptr if not found)
const Item* find(std::string_view name);

// Helper to register from any TU via static initialization
struct Registrar
{
    explicit Registrar(Item item);
};

} // namespace examples

// Macro to register an example function with a given name and help string
#define REGISTER_EXAMPLE(NAME, FN, HELP)                                                           \
    namespace                                                                                      \
    {                                                                                              \
    ::examples::Registrar _reg_##FN({NAME, FN, HELP});                                             \
    }
