#ifndef TEXTEDITORWINDOW_INCLUDED
#define TEXTEDITORWINDOW_INCLUDED

#include <SDL/SDL.h>
#include <iostream>
#include <string>
#include <algorithm>

#include "graphics/GraphicsManager.hpp"

class TextEditorWindow : public GraphicsManager
{
    private:
        bool _shift, _capslock;
        SDL_Surface *_target;

    public:
        TextEditorWindow() {}
        virtual ~TextEditorWindow() {}

        /**
         * Callback. Should be called whenever a key is pressed.
         *
         * @param key The key.
         * @param dir The direction of the event(up or down).
         */
        void onKeyDown(SDLKey &key, bool dir);

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
};

#endif // TEXTEDITORWINDOW_INCLUDED
