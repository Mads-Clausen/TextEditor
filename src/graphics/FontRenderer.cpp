#include "graphics/FontRenderer.hpp"

namespace graphics
{
    SDL_Surface *graphics::FontRenderer::_letter, *graphics::FontRenderer::_target;
    TTF_Font *graphics::FontRenderer::_font;
}
