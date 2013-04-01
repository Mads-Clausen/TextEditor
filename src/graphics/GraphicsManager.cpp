#include "graphics/GraphicsManager.hpp"

SDL_Surface *GraphicsManager::_screen;

GraphicsManager::GraphicsManager()
{

}

void GraphicsManager::render()
{

}

void GraphicsManager::init(int scrWidth, int scrHeight)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    _screen = SDL_SetVideoMode(scrWidth, scrHeight, 32, SDL_SWSURFACE | SDL_RESIZABLE);
}

void GraphicsManager::destroy()
{
    SDL_FreeSurface(_screen);
    SDL_Quit();
}
