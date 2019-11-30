#ifndef TERM_COLOR_H
#define TERM_COLOR_H

#include <string>
#include <iosfwd>

namespace Term
{

enum class Color
{
    None, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White, Count
};

enum class Effect
{
    Normal, Bold, Faint, Italic, Underline, SlowBlink, RapidBlink, Reverse, Count
};

inline Color to_color(size_t number)
{
    // exclude the Color::None
    return static_cast<Color>(number % (static_cast<size_t>(Color::Count) - 1) + 1);
}

std::string set_font(Color fg = Color::None, Color bg = Color::None, Effect ef = Effect::Normal)
{
    static const char * const prefix = "\033[";
    static const char * const suffix = "m";

    const std::string value = [fg, bg, ef]() -> std::string
    {
        if (fg == Color::None && bg == Color::None && ef == Effect::Normal)
            return "0";

        std::string result;
        if (fg != Color::None)
        {
            result += std::to_string(29 + static_cast<size_t>(fg));

            if (bg != Color::None || ef != Effect::Normal)
                result += ';';
        }

        if (bg != Color::None)
        {
            result += std::to_string(39 + static_cast<size_t>(bg));

            if (ef != Effect::Normal)
                result += ';';
        }

        if (ef != Effect::Normal)
            result += std::to_string(static_cast<size_t>(ef));

        return result;
    }();

    return prefix + value + suffix;
}

}

#endif
