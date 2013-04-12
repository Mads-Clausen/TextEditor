#ifndef SYNTAXLANGUAGE_INCLUDED
#define SYNTAXLANGUAGE_INCLUDED

#include <string>
#include <vector>
#include "graphics/Color.hpp"

namespace text
{
    class SyntaxLanguage
    {
        public:
            std::string path, name;
            graphics::ColorScheme colorScheme;
            std::vector<std::string> keywords;

            SyntaxLanguage()
            {
                path = "";
                name = "Plain Text";
            }

            SyntaxLanguage(const char *p, const char *n) : path(std::string(p)), name(std::string(n)) {}

            void load();
            void loadColors(const char *p);
    };
}

#endif // SYNTAXLANGUAGE_INCLUDED
