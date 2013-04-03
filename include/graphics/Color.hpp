#ifndef COLOR_INCLUDED
#define COLOR_INCLUDED

#include <initializer_list>

namespace graphics
{
    struct Color
    {
        int r, g, b, a;

        Color() : r(0), g(0), b(0), a(0) {}
        Color(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a) {}

        template<class T>
        Color& operator=(std::initializer_list<T> col)
        {
            std::initializer_list<int>::iterator it = col.begin();
            r = *it;
            ++it;
            g = *it;
            ++it;
            b = *it;

            return *this;
        }
};

    enum COLORSETS
    {
        COLORSET_DEFAULT,
        COLORSET_KEYWORD,
        COLORSET_NUMBERS,
        COLORSET_STRINGS,
        COLORSET_CHARS,
        COLORSET_COMMENTS,
        COLORSET_OPS
    };

    struct ColorScheme
    {
        Color
        defaultFG,  defaultBG,
        keywordsFG, keywordsBG,
        numbersFG,  numbersBG,
        stringsFG,  stringsBG,
        charsFG,    charsBG,
        commentsFG, commentsBG,
        opsFG,      opsBG;

        ColorScheme()
        {
            defaultFG = {255, 255, 255};
        }

        /**
         * Loads a colorscheme from a Lua file
         *
         * @param path The path to the script
         */
        void load(const char *path);
    };
}

#endif // COLOR_INCLUDED
