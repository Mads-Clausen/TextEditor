#include "graphics/TextEditorWindow.hpp"


TextEditorWindow::TextEditorWindow()
{

}

TextEditorWindow::~TextEditorWindow()
{

}

void TextEditorWindow::onKeyDown(SDLKey &key, bool dir)
{
    if(dir) // DOWN
    {
        if(key == SDLK_CAPSLOCK)
        {
            std::cout << "CAPS LOCK" << std::endl;
            _capslock = !_capslock;
            return;
        }

        char *kName = (char*) SDL_GetKeyName(key);

        if(_capslock && kName[0] && kName[0] > 96 && kName[0] < 123)
        {
            std::cout << "Capitalizing from " << kName[0] << " to " << (char) (kName[0] - 32) << std::endl;
            kName[0] = (char) (kName[0] - 32);
        }

        std::cout << "Key '" << kName << "' was pressed." << std::endl;
    }
    else
    {
        if(key == SDLK_CAPSLOCK)
        {
            std::cout << "CAPS LOCK OFF" << std::endl;
            _capslock = !_capslock;
            return;
        }

        std::string kName(SDL_GetKeyName(key));

        if(_capslock)
        {
            kName[0] = toupper(kName[0]);
        }

        std::cout << "Key '" << SDL_GetKeyName(key) << "' was released." << std::endl;
    }
}
