#ifndef FONTRENDERER_INCLUDED
#define FONTRENDERER_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>

#include "graphics/GraphicsManager.hpp"
#include "graphics/Color.hpp"

namespace graphics
{
    class FontRenderer
    {
        private:
            static SDL_Surface *_letter, *_target;
            static TTF_Font *_font;
        public:

            /**
             * Initialises variables in FontRenderer
             *
             * @return Success?
             */
            static bool init()
            {
                if(TTF_Init() == -1)
                {
                    std::cerr << "Failed to initalise SDL_ttf: " << TTF_GetError() << std::endl;
                    return false;
                }

                FontRenderer::setFont(TTF_OpenFont("fonts/default.ttf", 14));

                return true;
            }

            static void setFont(TTF_Font *f)
            {
                _font = f;
            }

            static void setTarget(SDL_Surface *t)
            {
                _target = t;
            }

            static void renderLetter(char letter, int x, int y, graphics::Color &col)
            {
                SDL_Rect offset;
                offset.x = x; offset.y = y;

                if(_letter != 0) SDL_FreeSurface(_letter);

                char text[2];
                text[0] = letter;
                text[1] = '\0';

                SDL_Color c;
                c.r = col.r; c.g = col.g; c.b = col.b;
                _letter = TTF_RenderText_Solid(_font, text, c);

                SDL_BlitSurface(_letter, 0, _target, &offset);
            }

            static void destroy()
            {
                if(_font != 0) TTF_CloseFont(_font);
                if(_letter != 0) SDL_FreeSurface(_letter);
            }
    };
}

#endif // FONTRENDERER_INCLUDED
