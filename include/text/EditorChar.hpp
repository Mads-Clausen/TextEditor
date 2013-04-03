#ifndef EDITORCHAR_INCLUDED
#define EDITORCHAR_INCLUDED

#include "graphics/Color.hpp"
namespace text
{
    struct EditorChar
    {
        graphics::Color fgColor, bgColor;
        char content;
    };
}

#endif // EDITORCHAR_INCLUDED
