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
                    else if(words[0] == "fg_keywods")   col = &keywordsFG;
                    else if(words[0] == "fg_strings")   col = &stringsFG;
                    else if(words[0] == "fg_numbers")   col = &numbersFG;
                    else if(words[0] == "fg_chars")     col = &charsFG;
                    else if(words[0] == "fg_operators") col = &opsFG;
                    else if(words[0] == "fg_comments")  col = &commentsFG;

                    else if(words[0] == "bg_default")   col = &defaultBG;
                    else if(words[0] == "bg_keywods")   col = &keywordsBG;
                    else if(words[0] == "bg_strings")   col = &stringsBG;
                    else if(words[0] == "bg_numbers")   col = &numbersBG;
                    else if(words[0] == "bg_chars")     col = &charsBG;
                    else if(words[0] == "bg_operators") col = &opsBG;
                    else if(words[0] == "bg_comments")  col = &commentsBG;

                    const char *r_s = words[1].c_str();
                    sscanf(r_s, "%i", &col->r);
                    const char *g_s = words[2].c_str();
                    sscanf(g_s, "%i", &col->g);
                    const char *b_s = words[3].c_str();
                    sscanf(b_s, "%i", &col->b);
                    const char *a_s = words[4].c_str();
                    sscanf(a_s, "%i", &col->a);

                    // std::cout << words[0].c_str() << " " << col->r << " " << col->g << " " << col->b << std::endl;
                }
            }

            file.close();
        }
    }
}
