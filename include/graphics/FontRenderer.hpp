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
            static SDL_Surface *_bg, *_letter, *_target;
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

                // Create new target, making sure to use the right architecture
                #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    _bg = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
                #else
                    _bg = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
                #endif

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
             * @param space Additional width to background color
             */
            static void renderLetter(char letter, int x, int y, graphics::Color &col, graphics::Color &bgCol, int space = 0)
            {
                space = 0;
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

                SDL_FreeSurface(_bg);

                int w, h;
                TTF_SizeText(_font, (char*) text, &w, &h);

                // Create new target, making sure to use the right architecture
                #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    _bg = SDL_CreateRGBSurface(SDL_SWSURFACE, w + space, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
                #else
                    _bg = SDL_CreateRGBSurface(SDL_SWSURFACE, w + space, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
                #endif

                SDL_FillRect(_bg, 0, SDL_MapRGB(_bg->format, bgCol.r, bgCol.g, bgCol.b));

                SDL_BlitSurface(_letter, 0, _bg, 0);
                SDL_BlitSurface(_bg, 0, _target, &offset);
            }

            static void destroy()
            {
                if(_letter != 0) SDL_FreeSurface(_letter);
            }
    };
}

#endif // FONTRENDERER_INCLUDED
