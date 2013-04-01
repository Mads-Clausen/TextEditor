#ifndef TEXTEDITORWINDOW_INCLUDED
#define TEXTEDITORWINDOW_INCLUDED

#include <SDL/SDL.h>
#include <iostream>
#include <string>
#include <algorithm>

class TextEditorWindow
{
    private:
        bool _shift, _capslock;

    public:
        TextEditorWindow();
        virtual ~TextEditorWindow();

        /**
         * Callback. Should be called whenever a key is pressed.
         *
         * @param key The key.
         * @param dir The direction of the event(up or down).
         */
        void onKeyDown(SDLKey &key, bool dir);
};

#endif // TEXTEDITORWINDOW_INCLUDED
