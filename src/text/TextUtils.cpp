#include "text/TextUtils.hpp"

#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>

#include "graphics/Color.hpp"

graphics::ColorScheme curCS;

namespace text
{
    namespace TextUtils
    {
        bool replace(std::string& str, const std::string& from, const std::string& to) {
            size_t start_pos = str.find(from);
            if(start_pos == std::string::npos)
                return false;
            str.replace(start_pos, from.length(), to);

            return true;
        }

        void toUppercase(char &c)
        {
            c = (c > 96 && c < 123 ? c - 32 : c);
        }

        void toLowercase(char &c)
        {
            c = (c > 64 && c < 91 ? c + 32 : c);
        }

        void toUppercase(std::string &s)
        {
            for(unsigned int i = 0; i < s.length(); ++i)
            {
                s[i] = (s[i] > 96 && s[i] < 123 ? s[i] - 32 : s[i]);
            }
        }

        void toLowercase(std::string &s)
        {
            for(unsigned int i = 0; i < s.length(); ++i)
            {
                s[i] = (s[i] > 64 && s[i] < 91 ? s[i] + 32 : s[i]);
            }
        }

        void applySyntaxHighlighting(std::string &s, std::vector<const char*> keywords)
        {
            for(unsigned int i = 0; i < keywords.size(); ++i)
            {
                std::stringstream temp;
                temp << "$" << graphics::COLORSET_KEYWORD << keywords[i] << "$" << graphics::COLORSET_DEFAULT;
                replace(s, std::string(keywords[i]), temp.str());
            }
        }

        std::vector<EditorChar> getEditorCharVector(std::string &s)
        {
            std::vector<EditorChar> chars;
            graphics::Color curBG = curCS.defaultBG, curFG = curCS.defaultFG;

            for(unsigned int i = 0; i < s.length(); ++i)
            {
                if(s[i] == '$' && isxdigit(s[i + 1]))
                {
                    int code;
                    // std::cout << "Scanning char " << s[i + 1] << std::endl;
                    char codeString[1];
                    codeString[0] = s[i + 1];
                    sscanf(codeString, "%i", &code);

                    switch(code)
                    {
                        case graphics::COLORSET_DEFAULT:
                            curBG = curCS.defaultBG; curFG = curCS.defaultFG;
                            break;
                        case graphics::COLORSET_KEYWORD:
                            curBG = curCS.keywordsBG; curFG = curCS.keywordsFG;
                            break;
                        case graphics::COLORSET_NUMBERS:
                            curBG = curCS.numbersBG; curFG = curCS.numbersFG;
                            break;
                        case graphics::COLORSET_CHARS:
                            curBG = curCS.charsBG; curFG = curCS.charsFG;
                            break;
                        case graphics::COLORSET_STRINGS:
                            curBG = curCS.stringsBG; curFG = curCS.stringsFG;
                            break;
                        case graphics::COLORSET_COMMENTS:
                            curBG = curCS.commentsBG; curFG = curCS.commentsFG;
                            break;
                        case graphics::COLORSET_OPS:
                            curBG = curCS.opsBG; curFG = curCS.opsFG;
                            break;
                    }

                    ++i;
                    continue;
                }

                EditorChar c;
                c.bgColor = curBG;
                c.fgColor = curFG;
                c.content = s[i];
                chars.push_back(c);
            }

            return chars;
        }

        void printEditorChars(std::vector<EditorChar> &chars)
        {
            for(unsigned int i = 0; i < chars.size(); ++i)
            {
                std::stringstream colorcode;
                colorcode << "\033[38;2;" << chars[i].fgColor.r << ";" << chars[i].fgColor.g << ";" << chars[i].fgColor.b
                    << "m\033[48;2;" << chars[i].bgColor.r << ";" << chars[i].bgColor.g << ";" << chars[i].bgColor.b << "m";

                std::cout << colorcode.str().c_str() << chars[i].content;
            }

            std::cout << std::endl;
        }

        void setColorScheme(graphics::ColorScheme sc)
        {
            curCS = sc;
        }
    }
}
