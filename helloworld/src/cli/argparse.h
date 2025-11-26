#pragma once

#include <cctype>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cli
{

class ArgParser
{
  public:
    explicit ArgParser(std::string program) : program_(std::move(program)) {}

    // Add a boolean flag (e.g., --verbose, -v)
    ArgParser& add_flag(std::string long_name, char short_name, std::string help)
    {
        Option opt;
        opt.long_name = std::move(long_name);
        opt.short_name = short_name;
        opt.takes_value = false;
        opt.help = std::move(help);
        options_.push_back(opt);
        index_option(options_.back());
        return *this;
    }

    // Add an option that takes a value. Default can be empty; required indicates it must be
    // provided.
    ArgParser& add_option(std::string long_name, char short_name, std::string help,
                          std::string default_value = {}, bool required = false)
    {
        Option opt;
        opt.long_name = std::move(long_name);
        opt.short_name = short_name;
        opt.takes_value = true;
        opt.default_value = std::move(default_value);
        opt.required = required;
        opt.help = std::move(help);
        options_.push_back(opt);
        index_option(options_.back());
        return *this;
    }

    // Declare a positional argument (for usage text). Not enforced strictly.
    ArgParser& add_positional(std::string name, std::string help, bool required = false)
    {
        pos_specs_.push_back({std::move(name), std::move(help), required});
        return *this;
    }

    // Parse argv. Returns false on error; call error() or usage() for details.
    bool parse(int argc, char** argv)
    {
        args_.clear();
        positionals_.clear();
        help_ = false;
        error_.clear();
        // Reset set flags
        for (auto& o : options_)
        {
            o.set = false;
            o.value.clear();
        }

        for (int i = 1; i < argc; ++i)
        {
            std::string token = argv[i];
            if (token == "--")
            {
                for (++i; i < argc; ++i)
                    positionals_.emplace_back(argv[i]);
                break;
            }
            if (token == "-h" || token == "--help")
            {
                help_ = true;
                continue;
            }
            if (token.size() > 2 && token.rfind("--", 0) == 0)
            {
                // Long option, possibly with =
                auto eq = token.find('=');
                std::string name =
                    token.substr(2, eq == std::string::npos ? std::string::npos : eq - 2);
                auto it = long_index_.find(name);
                if (it == long_index_.end())
                {
                    return fail_unknown("--" + name);
                }
                Option& opt = options_[it->second];
                if (!opt.takes_value)
                {
                    opt.set = true;
                }
                else
                {
                    std::string val;
                    if (eq != std::string::npos)
                    {
                        val = token.substr(eq + 1);
                    }
                    else
                    {
                        if (i + 1 >= argc)
                            return fail_required_value("--" + name);
                        val = argv[++i];
                    }
                    opt.value = std::move(val);
                    opt.set = true;
                }
            }
            else if (token.size() >= 2 && token[0] == '-' && token[1] != '-')
            {
                // Short option: -x value (no bundling)
                if (token.size() != 2)
                    return fail("short option bundling not supported: " + token);
                char c = token[1];
                auto it = short_index_.find(c);
                if (it == short_index_.end())
                {
                    return fail_unknown(std::string("-") + c);
                }
                Option& opt = options_[it->second];
                if (!opt.takes_value)
                {
                    opt.set = true;
                }
                else
                {
                    if (i + 1 >= argc)
                        return fail_required_value(std::string("-") + c);
                    opt.value = argv[++i];
                    opt.set = true;
                }
            }
            else
            {
                positionals_.push_back(std::move(token));
            }
        }

        // Check required options
        for (const auto& o : options_)
        {
            if (o.required && !o.set && o.default_value.empty())
            {
                return fail("missing required option --" + o.long_name);
            }
        }
        return true;
    }

    bool help() const
    {
        return help_;
    }
    const std::string& error() const
    {
        return error_;
    }

    bool get_flag(std::string_view name) const
    {
        const auto* o = find_long(name);
        return o ? o->set : false;
    }
    std::string get_string(std::string_view name, std::string def = {}) const
    {
        const auto* o = find_long(name);
        if (!o)
            return def;
        if (o->takes_value)
            return o->set ? o->value : (!o->default_value.empty() ? o->default_value : def);
        return o->set ? std::string("true") : def;
    }
    int get_int(std::string_view name, int def = 0) const
    {
        const auto s = get_string(name, std::to_string(def));
        try
        {
            size_t pos = 0;
            int v = std::stoi(s, &pos);
            (void)pos;
            return v;
        }
        catch (...)
        {
            return def;
        }
    }
    double get_double(std::string_view name, double def = 0.0) const
    {
        const auto s = get_string(name, std::to_string(def));
        try
        {
            size_t pos = 0;
            double v = std::stod(s, &pos);
            (void)pos;
            return v;
        }
        catch (...)
        {
            return def;
        }
    }

    const std::vector<std::string>& positionals() const
    {
        return positionals_;
    }

    std::string usage() const
    {
        std::ostringstream os;
        os << "Usage: " << program_ << " [options]";
        for (const auto& p : pos_specs_)
        {
            os << ' ' << (p.required ? '<' : '[') << p.name << (p.required ? '>' : ']');
        }
        os << "\n\nOptions:\n";
        os << "  -h, --help                 Show this help\n";
        for (const auto& o : options_)
        {
            os << "  ";
            if (o.short_name)
                os << '-' << o.short_name << ", ";
            else
                os << "    ";
            os << "--" << o.long_name;
            if (o.takes_value)
                os << " <value>";
            os << "\n      " << o.help;
            if (!o.default_value.empty())
                os << " (default: " << o.default_value << ')';
            if (o.required)
                os << " [required]";
            os << "\n";
        }
        if (!pos_specs_.empty())
        {
            os << "\nPositionals:\n";
            for (const auto& p : pos_specs_)
            {
                os << "  " << p.name << "\n      " << p.help << (p.required ? " [required]" : "")
                   << "\n";
            }
        }
        return os.str();
    }

  private:
    struct Option
    {
        std::string long_name;
        char short_name = 0;
        bool takes_value = false;
        std::string default_value;
        bool required = false;
        std::string help;
        bool set = false;
        std::string value;
    };
    struct PosSpec
    {
        std::string name;
        std::string help;
        bool required = false;
    };

    void index_option(const Option& o)
    {
        long_index_[o.long_name] = options_.size() - 1U;
        if (o.short_name)
            short_index_[o.short_name] = options_.size() - 1U;
    }

    const Option* find_long(std::string_view name) const
    {
        auto it = long_index_.find(std::string(name));
        if (it == long_index_.end())
            return nullptr;
        return &options_[it->second];
    }

    bool fail(std::string msg)
    {
        error_ = std::move(msg);
        return false;
    }
    bool fail_unknown(const std::string& opt)
    {
        return fail("unknown option: " + opt);
    }
    bool fail_required_value(const std::string& opt)
    {
        return fail("missing value for option: " + opt);
    }

    std::string program_;
    std::vector<Option> options_;
    std::vector<PosSpec> pos_specs_;
    std::map<std::string, size_t> long_index_;
    std::map<char, size_t> short_index_;

    std::vector<std::string> args_;
    std::vector<std::string> positionals_;
    bool help_ = false;
    std::string error_;
};

} // namespace cli
