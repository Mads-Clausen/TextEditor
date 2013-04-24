#include "graphics/FontRenderer.hpp"

namespace graphics
{
    SDL_Surface *graphics::FontRenderer::_letter, *graphics::FontRenderer::_target, *graphics::FontRenderer::_bg;
    TTF_Font *graphics::FontRenderer::_font;
}
