#include "examples/registry.h"
#include "logger.h"

#include <string>
#include <string_view>
#include <vector>

static int run_example(const examples::Item& ex, const std::vector<std::string>& args)
{
    // Build argv with argv[0] = example name
    std::vector<std::string> storage;
    storage.reserve(args.size() + 1);
    storage.push_back(ex.name);
    for (const auto& s : args)
        storage.push_back(s);
    std::vector<char*> argv;
    argv.reserve(storage.size());
    for (auto& s : storage)
        argv.push_back(s.data());
    return ex.fn(static_cast<int>(argv.size()), argv.data());
}

int main(int argc, char** argv)
{
    logger::Logger log{logger::Level::INFO, "[runner] {}"};

    // Parse global args: --list, --example <name>, --args <...>  (or use -- to pass the rest)
    bool list = false;
    std::string example_name; // empty or "all" means run all
    std::vector<std::string> example_args;

    for (int i = 1; i < argc; ++i)
    {
        std::string a = argv[i];
        if (a == "--list")
        {
            list = true;
        }
        else if (a == "--example" && i + 1 < argc)
        {
            example_name = argv[++i];
        }
        else if (a == "--args")
        {
            for (++i; i < argc; ++i)
                example_args.emplace_back(argv[i]);
            break;
        }
        else if (a == "--")
        {
            for (++i; i < argc; ++i)
                example_args.emplace_back(argv[i]);
            break;
        }
        else
        {
            log.warn("unknown option '{}' (use --list or --example <name> --args <...>)", a);
        }
    }

    const auto& all = examples::all();
    if (list)
    {
        log.info("available examples ({}):", all.size());
        for (const auto& it : all)
        {
            log.info("- {}: {}", it.name, it.help);
        }
        return 0;
    }

    if (example_name.empty() || example_name == "all")
    {
        if (all.empty())
        {
            log.error("no examples registered");
            return 1;
        }
        int last_rc = 0;
        for (const auto& it : all)
        {
            log.info("running example: {}", it.name);
            last_rc = run_example(it, example_args);
            if (last_rc != 0)
            {
                log.error("example '{}' failed with {}", it.name, last_rc);
            }
        }
        return last_rc;
    }

    if (const auto* it = examples::find(example_name))
    {
        log.info("running example: {}", it->name);
        return run_example(*it, example_args);
    }

    log.error("unknown example: '{}' (use --list)", example_name);
    return 2;
}
