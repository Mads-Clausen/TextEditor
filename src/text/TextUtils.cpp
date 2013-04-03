#include "text/TextUtils.hpp"

#include <sstream>
#include <algorithm>
#include "graphics/Color.hpp"

ColorScheme curCS;

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
            temp << "&[" << curCS.keywordsFG.r << "," << curCS.keywordsFG.g << "," << curCS.keywordsFG.b << "," << curCS.keywordsFG.a << ":"
                << curCS.keywordsBG.r << "," << curCS.keywordsBG.g << "," << curCS.keywordsBG.b << "," << curCS.keywordsBG.a << "]" << keywords[i];
            temp << "&[" << curCS.defaultFG.r << "," << curCS.defaultFG.g << "," << curCS.defaultFG.b << "," << curCS.defaultFG.a << ":"
                << curCS.defaultBG.r << "," << curCS.defaultBG.g << "," << curCS.defaultBG.b << "," << curCS.defaultBG.a << "]";
            replace(s, std::string(keywords[i]), temp.str());
        }
    }

    void getEditorCharVector(std::string &s)
    {

    }
}
