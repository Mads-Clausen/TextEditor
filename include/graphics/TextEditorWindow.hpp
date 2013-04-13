#ifndef TEXTEDITORWINDOW_INCLUDED
#define TEXTEDITORWINDOW_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
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
            int _caretWait, _fontSize, _spacing, _scrollY, _fontHeight;
            std::string _saveFile;
            std::vector<std::string> _hlLines;

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
             * Callback. Should be called whenever a key event occured.
             *
             * @param key The key.
             * @param dir The direction of the event(up or down).
             */
            void onKeyEvent(SDL_KeyboardEvent &key, bool dir);

            /**
             * Callback. Should be called whenever a mouse event happens.
             *
             * @param ev The event.
             * @param dir Up or down?
             */
             void onMouseEvent(SDL_MouseButtonEvent &ev, bool dir);

             /**
              * Scroll up/down.
              *
              * @param dir Up or down?
              */
              void scroll(bool dir);

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
             * Inserts a string of text.
             *
             * @param s The string to insert.
             */
            void insertText(std::string s);

            /**
             * Adds a line.
             */
            void addLine();

            /**
             * See documentation for graphics::GraphicsManager::render()
             */
            void render();

            /**
             * Updates stuff.
             */
            void update();

            /**
             * Returns a vector with the lines.
             *
             * @return A vector with lines represented as std::strings
             */
            std::vector<std::string> getLines();

            /**
             * Gets the total height of the text with the current font.
             *
             * @param end Where to end.
             *
             * @return The total height in pixels.
             */
            int getTotalHeight(int end);

            /**
             * Saves text to file.
             */
            void save();
    };
}

#endif // TEXTEDITORWINDOW_INCLUDED
