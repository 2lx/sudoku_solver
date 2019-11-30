#ifndef TERM_COLOR_H
#define TERM_COLOR_H

#include <string>
#include <iosfwd>

enum class Color
{
    None, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White, Count
};

inline Color to_color(size_t number)
{
    // exclude the Color::None
    return static_cast<Color>(number % (static_cast<size_t>(Color::Count) - 1) + 1);
}

std::string set_color(Color fg = Color::None, Color bg = Color::None)
{
    if (fg == Color::None && bg == Color::None)
        return "\033[0m";

    return "\033["
        + std::to_string(29 + static_cast<size_t>(fg)) + ';'
        + std::to_string(39 + static_cast<size_t>(bg)) + 'm';
}

std::ostream & set_color(std::ostream & ostr, Color fg = Color::None, Color bg = Color::None)
{
    return ostr << set_color(fg, bg);
}

#endif
