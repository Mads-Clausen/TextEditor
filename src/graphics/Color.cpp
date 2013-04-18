
#include "graphics/Color.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

namespace graphics
{
    void ColorScheme::load(const char *path)
    {
        std::string line;
        std::ifstream file(path);
        if(file.is_open())
        {
            while(file.good())
            {
                getline(file, line);
                std::istringstream iss(line);
                std::vector<std::string> words;

                do
                {
                    std::string sub;
                    iss >> sub;
                    words.push_back(sub);
                } while (iss);

                if(words[0].length() > 3 && words[0] != "#")
                {
                    Color *col;
                    if(words[0] == "fg_default")        col = &defaultFG;
                    else if(words[0] == "fg_keywords")   col = &keywordsFG;
                    else if(words[0] == "fg_strings")   col = &stringsFG;
                    else if(words[0] == "fg_numbers")   col = &numbersFG;
                    else if(words[0] == "fg_chars")     col = &charsFG;
                    else if(words[0] == "fg_operators") col = &opsFG;
                    else if(words[0] == "fg_comments")  col = &commentsFG;
                    else if(words[0] == "fg_preprocess") col = &preprocFG;
                    else if(words[0] == "fg_caret")      col = &caretFG;

                    else if(words[0] == "bg_default")   col = &defaultBG;
                    else if(words[0] == "bg_keywords")   col = &keywordsBG;
                    else if(words[0] == "bg_strings")   col = &stringsBG;
                    else if(words[0] == "bg_numbers")   col = &numbersBG;
                    else if(words[0] == "bg_chars")     col = &charsBG;
                    else if(words[0] == "bg_operators") col = &opsBG;
                    else if(words[0] == "bg_comments")  col = &commentsBG;
                    else if(words[0] == "bg_preprocess") col = &preprocBG;
                    else if(words[0] == "bg_caret")      col = &caretBG;

                    sscanf(words[1].c_str(), "%i", &(col->r));
                    sscanf(words[2].c_str(), "%i", &(col->g));
                    sscanf(words[3].c_str(), "%i", &(col->b));
                    sscanf(words[4].c_str(), "%i", &(col->a));
                    sscanf(words[5].c_str(), "%i", &(col->term));

                    // std::cout << "Default FG color = " << defaultFG.term << std::endl;
                }
            }

            file.close();
        }
    }
}
