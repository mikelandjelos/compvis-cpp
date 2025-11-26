/**
 * \file
 * \ingroup engine
 * Minimal registry to collect example functions and look them up by name.
 */
#pragma once

#include <string>
#include <string_view>
#include <vector>

/** \defgroup engine Engine
 *  Core plumbing for the example runner (registry + bootstrap).
 */

namespace examples
{

//! All examples use this function signature
using ExampleFn = int(int argc, char** argv);

struct Item
{
    const char* name; // unique id (e.g., "show")
    ExampleFn* fn;    // function to run
    const char* help; // short description
};

//! Register a new example (used by the Registrar helper)
void register_example(const Item& item);

//! Access all registered examples
const std::vector<Item>& all();

//! Find an example by name (or nullptr if not found)
const Item* find(std::string_view name);

//! Helper to register from any translation unit via static init
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
