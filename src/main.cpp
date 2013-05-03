#include <vector>
#include <iostream>

#include <SDL/SDL.h>
#include <string>
#include <sstream>

#include "graphics/GraphicsManager.hpp"
#include "graphics/TextEditorWindow.hpp"
#include "graphics/FontRenderer.hpp"
#include "text/TextUtils.hpp"
#include "Timer.hpp"

graphics::TextEditorWindow teWin;

Timer fps, update;
unsigned int frame;

int main(int argc, char **argv)
{
    if(!graphics::GraphicsManager::init("Da VIMci", 1024, 900) || !graphics::FontRenderer::init())
        return false;

    update.start();
    fps.start();

    teWin.init();
    teWin.setActive(true);

    char path[4096];
    printf("Enter a file to open: ");
    scanf("%s", path);
    teWin.load(path);
    teWin.render();
    graphics::GraphicsManager::flip();

    for(bool running = true; running;)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_PAGEUP)
                    {
                        teWin.setActive(true);
                        break;
                    }
                    else if(event.key.keysym.sym == SDLK_PAGEDOWN)
                    {
                        // teWin.setActive(false);
                        break;
                    }

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
                case SDL_MOUSEBUTTONUP:
                    teWin.onMouseEvent(event.button, false);
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        // Calculte FPS
        ++frame;
        if(update.get_ticks() > 1000)
        {
            std::stringstream caption;
            caption << frame / (fps.get_ticks() / 1000.f);
            SDL_WM_SetCaption((std::string("Da VIMci - FPS: ") + caption.str()).c_str(), 0);

            update.start();
        }

        teWin.update();
        graphics::GraphicsManager::flip();
    }

    graphics::GraphicsManager::destroy();
    graphics::FontRenderer::destroy();

    return 0;
}
