#ifndef TEXTEDITORWINDOW_INCLUDED
#define TEXTEDITORWINDOW_INCLUDED

#include <SDL/SDL.h>
#include <iostream>
#include <string>
#include <algorithm>

#include "graphics/GraphicsManager.hpp"
#include "text/TextUtils.hpp"
#include "text/SyntaxLanguage.hpp"
using namespace text::TextUtils;

typedef std::vector<char> CharList;
typedef std::vector<CharList*> CharMap;

class TextEditorWindow : public GraphicsManager
{
    private:
        bool _shift, _capslock;
        CharMap _lines; // Vectorception?
        unsigned int _cursorX, _cursorY, _tabLen;
        SDL_Surface *_target;
        graphics::ColorScheme _colors;
        text::SyntaxLanguage _lang;

        void moveCursorDown();
        void moveCursorUp();
        void moveCursorLeft();
        void moveCursorRight();

    public:
        TextEditorWindow()
        {
            _cursorX = _cursorY = 0;
        }

        virtual ~TextEditorWindow();

        /**
         * Callback. Should be called whenever a key is pressed.
         *
         * @param key The key.
         * @param dir The direction of the event(up or down).
         */
        void onKeyEvent(SDL_KeyboardEvent &key, bool dir);

        /**
         * Initialises the "window".
         *
         * @param w The width.
         * @param h The height.
         *
         * @return Success?
         */
        bool init(int w = _screen->w, int h = _screen->h);

        /**
         * Resizes the "window".
         *
         * @param w The width.
         * @param h The height.
         *
         * @return Success?
         */
        bool resize(int w, int h);

        /**
         * Adds a char at the current cursor position.
         *
         * @param c The char to add.
         */
        void addChar(char c);

        /**
         * Removes the char at the current cursor position.
         */
        void removeChar();

        /**
         * Adds a line.
         */
        void addLine();
};

#endif // TEXTEDITORWINDOW_INCLUDED
