#include <vector>
#include <iostream>

#include <SDL/SDL.h>

#include "graphics/GraphicsManager.hpp"
#include "graphics/TextEditorWindow.hpp"
#include "text/TextUtils.hpp"

TextEditorWindow teWin;

int main(int argc, char **argv)
{
    GraphicsManager::init(800, 600);
    teWin.init();
    std::cout << "Done initing" << std::endl;

    std::string test("for(unsigned int i = 0; i < something; ++i)");
    std::vector<const char*> keywords;
    keywords.push_back("for");
    keywords.push_back("unsigned");
    keywords.push_back("int");

    TextUtils::applySyntaxHighlighting(test, keywords);
    std::cout << test.c_str() << std::endl;

    for(bool running = true; running;)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_KEYDOWN:
                    teWin.onKeyDown(event.key.keysym.sym, true);
                    break;
                case SDL_KEYUP:
                    teWin.onKeyDown(event.key.keysym.sym, false);
                    break;
                case SDL_QUIT:
                    exit(0);
            }
        }
    }

    GraphicsManager::destroy();

    return 0;
}
