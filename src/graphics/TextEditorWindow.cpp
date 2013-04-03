#include "graphics/TextEditorWindow.hpp"

bool TextEditorWindow::init(int w, int h)
{
    bool r = this->resize(w, h);
    std::cout << "Initialised TextEditorWindow with size " << w << "*" << h << std::endl;
    return r;
}

bool TextEditorWindow::resize(int w, int h)
{
    if(_target != 0) SDL_FreeSurface(_target);

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        _target = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    #else
        _target = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    #endif


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
    if(_capslock)
        text::TextUtils::toUppercase(kName);

    if(dir) // DOWN
    {
        std::cout << "Key '" << kName.c_str() << "' was pressed." << std::endl;
    }
    else // UP
    {
        std::cout << "Key '" << kName.c_str() << "' was released." << std::endl;
    }
}
