#ifndef COLOR_INCLUDED
#define COLOR_INCLUDED

#include <initializer_list>
#include <string>

namespace graphics
{
    struct Color
    {
        int r, g, b, a, term;

        Color() : r(0), g(0), b(0), a(0) {}
        Color(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a) {}

        Color& operator=(Color &col)
        {
            r = col.r;
            g = col.g;
            b = col.b;
            a = col.a;
            term = col.term;

            return *this;
        }

        template<class T>
        Color& operator=(std::initializer_list<T> col)
        {
            std::initializer_list<int>::iterator it = col.begin();
            r = *it;
            ++it;
            g = *it;
            ++it;
            b = *it;

            term = 7; // White

            return *this;
        }
};

    enum COLORSETS
    {
        COLORSET_DEFAULT = 0,
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
