#ifndef GRAPHICSMANAGER_INCLUDED
#define GRAPHICSMANAGER_INCLUDED

#include <SDL/SDL.h>
#include <iostream>
#include <math.h>

#include "graphics/Color.hpp"

namespace graphics
{
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
            virtual void render() {}

            /**
             * Flips the buffers. This will make all changes made to the global rendertarget visible in the window.
             */
             static void flip()
             {
                 SDL_Flip(_screen);
             }

            /**
             * Sets the global rendertaget. NOT TO BE OVERWRITTEN.
             *
             * @param target The new rendertarget.
             */
            static void setGlobalRendertarget(SDL_Surface *target)
            {
                _screen = target;
            }

            /**
             * Initialises the global rendertarget.
             *
             * @param title The window title.
             * @param scrWidth The width of the global rendertarget(width of the window).
             * @param scrHeight The height of the global rendertarget(height of the window).
             *
             * @return Success?
             */
            static bool init(const char *title, int scrWidth, int scrHeight);

            /**
             * Destroys the global rendertarget.
             */
            static void destroy();
    };

    void plot(SDL_Surface *surf, int x, int y, Color col);

    void line(SDL_Surface *surf, int x1, int y1, int x2, int y2, Color col);
}

#endif // GRAPHICSMANAGER_INCLUDED
