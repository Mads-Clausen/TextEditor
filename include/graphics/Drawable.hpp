#ifndef DRAWABLE_INCLUDED
#define DRAWABLE_INCLUDED

/**
 * The base class for all drawable elements
 */
class Drawable
{
    public:
        /**
         * Renders to the SDL_Surface target.
         *
         * @param target the rendertarget.
         */
        virtual void render(SDL_Surface *target);
}

#endif // DRAWABLE_INCLUDED
