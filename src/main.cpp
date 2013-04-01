#include <SDL/SDL.h>

#include "graphics/GraphicsManager.hpp"
#include "graphics/TextEditorWindow.hpp"

TextEditorWindow teWin;

int main(int argc, char **argv)
{
    GraphicsManager::init(800, 600);
    teWin.init();

    for(bool running = true; running;)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_KEYDOWN:
                    teWin.onKeyDown(event.key.keysym.sym, true);
                    break;
                case SDL_KEYUP:
                    teWin.onKeyDown(event.key.keysym.sym, false);
                    break;
                case SDL_QUIT:
                    exit(0);
            }
        }
    }

    GraphicsManager::destroy();

    return 0;
}
