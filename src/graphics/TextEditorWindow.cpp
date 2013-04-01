#include "graphics/TextEditorWindow.hpp"

void toUpper(char &c)
{
    if(c > 96 && c < 123)
    {
        c -= 32;
    }
}

bool TextEditorWindow::init(int w, int h)
{
    bool r = this->resize(w, h);
    std::cout << "Initialised window with size " << w << "*" << h << std::endl;
    return r;
}

bool TextEditorWindow::resize(int w, int h)
{
    if(_target != 0) SDL_FreeSurface(_target);
    Uint32 rmask, gmask, bmask, amask;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    _target = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, rmask, gmask, bmask, amask);
    if(_target == 0) {
        std::cerr << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void TextEditorWindow::onKeyDown(SDLKey &key, bool dir)
{
    if(key == SDLK_CAPSLOCK)
    {
        _capslock = !_capslock;
        return;
    }

    std::string kName(SDL_GetKeyName(key));
    if(_capslock && kName[0] && kName[0] > 96 && kName[0] < 123)
        toUpper(kName[0]);

    if(dir) // DOWN
    {
        std::cout << "Key '" << kName.c_str() << "' was pressed." << std::endl;
    }
    else
    {
        std::cout << "Key '" << kName.c_str() << "' was released." << std::endl;
    }
}
