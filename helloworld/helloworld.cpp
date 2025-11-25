#include <fmt/color.h>

int main()
{
    std::string hwString = fmt::format("{1}, {0}!\n", "World", "Hello");
    fmt::print(fg(fmt::color::green_yellow) | fmt::emphasis::bold,
               hwString);
    return 0;
}