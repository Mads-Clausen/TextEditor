#include "text/SyntaxLanguage.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace text
{
    void SyntaxLanguage::load()
    {
        if(path == "")
            return;

        // Load keywords
        std::ifstream file((path + "/keywords.txt").c_str());
        if(file.is_open())
        {
            std::string line;

            while(file.good())
            {
                getline(file, line);
                keywords.push_back(line);
            }

            keywords.pop_back(); // Delete last element, which is always an empty string
        }
    }

    void SyntaxLanguage::loadColors(char *p)
    {
        colorScheme.load(p);
    }
}
