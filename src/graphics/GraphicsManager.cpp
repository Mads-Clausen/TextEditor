#include <SDL/SDL_ttf.h>

#include "graphics/GraphicsManager.hpp"

namespace graphics
{
    SDL_Surface *GraphicsManager::_screen;

    GraphicsManager::GraphicsManager()
    {

    }

    bool GraphicsManager::init(const char *title, int scrWidth, int scrHeight)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        {
            std::cerr << "Failed to initialise SDL: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_EnableUNICODE(1);

        _screen = SDL_SetVideoMode(scrWidth, scrHeight, 32, SDL_SWSURFACE | SDL_RESIZABLE);
        if(_screen == 0)
        {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_WM_SetCaption(title, 0);

        return true;
    }

    void GraphicsManager::destroy()
    {
        SDL_FreeSurface(_screen);
        TTF_Quit();
        SDL_Quit();
    }

    void plot(SDL_Surface *surf, int x, int y, graphics::Color col)
    {
        int color = SDL_MapRGB(surf->format, col.r, col.g, col.b);
        Uint32 *framebuffer = (Uint32*) surf->pixels;
        y %= surf->w;
        x %= surf->h;
        framebuffer[y * surf->w + x ] = color;
    }

    void line(SDL_Surface *surf, int x1, int y1, int x2, int y2, Color col)
    {
        int x = x2 - x1, y = y2 - y1;
        int len = sqrt(x*x + y*y);
        int stepX = x / len, stepY = y / len;
        x = x1;
        y = y1;

        for(int i = 0; i < len; ++i)
        {
            plot(surf, x, y, col);
            x += stepX;
            y += stepY;
        }
    }
}
