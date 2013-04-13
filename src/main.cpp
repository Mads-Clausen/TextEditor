#include <vector>
#include <iostream>

#include <SDL/SDL.h>

#include "graphics/GraphicsManager.hpp"
#include "graphics/TextEditorWindow.hpp"
#include "graphics/FontRenderer.hpp"
#include "text/TextUtils.hpp"

graphics::TextEditorWindow teWin;

int main(int argc, char **argv)
{
    if(!graphics::GraphicsManager::init("Da VIMci", 800, 600) || !graphics::FontRenderer::init())
        return false;

    teWin.init();

    for(bool running = true; running;)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_KEYDOWN:
                    teWin.onKeyEvent(event.key, true);
                    break;
                case SDL_KEYUP:
                    teWin.onKeyEvent(event.key, false);
                    break;
                case SDL_VIDEORESIZE:
                    teWin.resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    teWin.onMouseEvent(event.button, true);
                    break;
                case SDL_QUIT:
                    exit(0);
                    break;
            }
        }

        teWin.update();
        graphics::GraphicsManager::flip();
    }

    graphics::GraphicsManager::destroy();
    graphics::FontRenderer::destroy();

    return 0;
}
