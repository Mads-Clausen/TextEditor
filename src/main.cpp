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

graphics::TextEditorWindow teWin, teWin2;

Timer fps, update;
unsigned int frame;

int main(int argc, char **argv)
{
    if(!graphics::GraphicsManager::init("Da VIMci", 800, 600) || !graphics::FontRenderer::init())
        return false;

    update.start();
    fps.start();

    teWin.init(375, 600);
    teWin2.init(375, 600);
    teWin2.setPosition(425, 0);

    teWin2.setActive(true);
    teWin.setActive(true);

    teWin.render();
    teWin2.render();

    teWin2.setActive(false);

    for(bool running = true; running;)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_PAGEUP)
                    {
                        teWin2.setActive(false);
                        teWin.setActive(true);
                        teWin.render();
                        break;
                    }
                    else if(event.key.keysym.sym == SDLK_PAGEDOWN)
                    {
                        teWin.setActive(false);
                        teWin2.setActive(true);
                        teWin2.render();
                        break;
                    }

                    teWin.onKeyEvent(event.key, true);
                    teWin2.onKeyEvent(event.key, true);
                    break;
                case SDL_KEYUP:
                    teWin.onKeyEvent(event.key, false);
                    teWin2.onKeyEvent(event.key, false);
                    break;
                case SDL_VIDEORESIZE:
                    teWin.resize(event.resize.w, event.resize.h);
                    teWin2.resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    teWin.onMouseEvent(event.button, true);
                    teWin2.onMouseEvent(event.button, true);
                    break;
                case SDL_MOUSEBUTTONUP:
                    teWin.onMouseEvent(event.button, false);
                    teWin2.onMouseEvent(event.button, false);
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
            caption << "Average Frames Per Second: " << frame / (fps.get_ticks() / 1000.f);
            SDL_WM_SetCaption((std::string("Da VIMci - FPS: ") + caption.str()).c_str(), 0);

            update.start();
        }

        teWin.update();
        teWin2.update();
        graphics::GraphicsManager::flip();
    }

    graphics::GraphicsManager::destroy();
    graphics::FontRenderer::destroy();

    return 0;
}
