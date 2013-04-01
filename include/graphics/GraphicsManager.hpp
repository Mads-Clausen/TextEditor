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
        static SDL_Surface *_screen;

    public:
        /**
         * Constructor
         */
        GraphicsManager();

        /**
         * Renders to the global rendertarget.
         */
        virtual void render();

        /**
         * Sets the global rendertaget. NOT TO BE OVERWRITTEN.
         *
         * @param target The new rendertarget.
         */
        void setGlobalRendertarget(SDL_Surface *target)
        {
            _screen = target;
        }

        /**
         * Initialises the global rendertarget.
         *
         * @param scrWidth The width of the global rendertarget(width of the window).
         * @param scrHeight The height of the global rendertarget(height of the window).
         */
        static void init(int scrWidth, int scrHeight);

        /**
         * Destroys the global rendertarget.
         */
        static void destroy();
};

#endif // GRAPHICSMANAGER_INCLUDED
