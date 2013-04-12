#ifndef TEXTEDITORWINDOW_INCLUDED
#define TEXTEDITORWINDOW_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <string>
#include <algorithm>

#include "graphics/GraphicsManager.hpp"
#include "graphics/FontRenderer.hpp"
#include "text/TextUtils.hpp"
#include "text/SyntaxLanguage.hpp"
using namespace text::TextUtils;

namespace graphics
{
    typedef std::vector<char> CharList;
    typedef std::vector<CharList*> CharMap;

    class TextEditorWindow : public GraphicsManager
    {
        private:
            bool _shift, _capslock;
            CharMap _lines; // Vectorception?
            unsigned int _cursorX, _cursorY, _tabLen, _posX, _posY;
            SDL_Surface *_target;
            graphics::ColorScheme _colors;
            text::SyntaxLanguage _lang;
            TTF_Font *_font;
            bool _caretVisible;
            int _caretWait;

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

            /**
             * See documentation for graphics::GraphicsManager::render()
             */
            void render();

            /**
             * Returns a vector with the lines.
             *
             * @return A vector with lines represented as std::strings
             */
            std::vector<std::string> getLines();
    };
}

#endif // TEXTEDITORWINDOW_INCLUDED
