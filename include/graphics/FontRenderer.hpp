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

                char *tmp = "";
                SDL_Color c;
                c.r = c.g = c.b = 0;
                _letter = TTF_RenderText_Solid(_font, tmp, c);

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

            /**
             * Renders a letter to _target at (x, y) with given colors
             *
             * @param letter The letter to render
             * @param x The x coordinate
             * @param y The y coordinate
             * @param col The foreground color
             * @param bgCol the background color
             */
            static void renderLetter(char letter, int x, int y, graphics::Color &col, graphics::Color &bgCol)
            {
                SDL_Rect offset;
                offset.x = x; offset.y = y;

                if(_letter != 0)
                {
                    SDL_FreeSurface(_letter);
                    _letter = 0;
                }

                // SDL_FillRect(_letter, 0, SDL_MapRGB(_letter->format, bgCol.r, bgCol.g, bgCol.b));

                Uint16 text[2];
                text[0] = letter;
                text[1] = '\0';

                SDL_Color c;
                c.r = col.r; c.g = col.g; c.b = col.b;
                _letter = TTF_RenderUNICODE_Blended(_font, text, c);

                SDL_BlitSurface(_letter, 0, _target, &offset);
            }

            static void destroy()
            {
                if(_letter != 0) SDL_FreeSurface(_letter);
            }
    };
}

#endif // FONTRENDERER_INCLUDED
