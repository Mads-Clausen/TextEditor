#ifndef GRAPHICSMANAGER_INCLUDED
#define GRAPHICSMANAGER_INCLUDED

#include <SDL/SDL.h>


/**
 * Base class for Graphics Managers(like Windows, Scrollboxes, etc.)
 */
class GraphicsManager
{
    protected:
        /**
         * The SDL_Surface used for rendering(global).
         */
        static SDL_Surface *screen;

        /**
         * The SDL_Surface used for "local" rendering.
         */
        SDL_Surface *target;


    public:
        /**
         * Constructor
         */
        GraphicsManager();

        /**
         * Renders to the static rendertarget(GraphicsManager::screen).
         */
        virtual void render() {}
};

#endif // GRAPHICSMANAGER_INCLUDED
