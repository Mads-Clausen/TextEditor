#ifndef COLOR_INCLUDED
#define COLOR_INCLUDED

struct Color
{
    float r, g, b, a;

    Color() : r(0), g(0), b(0), a(0) {}
    Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
};

struct ColorScheme
{
    Color
    defaultFG,  defaultBG,
    keywordsFG, keywordsBG,
    numbersFG,  numberBG,
    stringsFG,  stringBG,
    charsFG,    charsBG,
    commentsFG, commentsBG,
    opSFG,      OPSbg;
};

#endif // COLOR_INCLUDED
