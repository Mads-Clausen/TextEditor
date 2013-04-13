#include "text/TextUtils.hpp"

#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "graphics/Color.hpp"

const std::string alphanums("`abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.'");
const std::string operators("+-*/{}()[],.^&%!=|<>:;?");

int countChars(std::string s, const char *needle)
{
    int count = 0;
    bool inString = false, inChar = false;

    for(unsigned int i = 0; i < s.length(); ++i)
    {
        // Make sure it's not escaped
        if(i > 0)
        {
            if(s[i - 1] == '\\')
                continue;
        }

        if(s[i] == needle[0] && (needle[0] == '"' ? true : !inString) && (needle[0] == '\'' ? true : !inChar))
        {
            ++count;
        }

        if(s[i] == '"' && !inChar)
        {
            if(i > 0)
            {
                if(s[i - 1] == '\\')
                    continue;
            }

            inString = !inString;
        }

        if(s[i] == '\'' && !inString)
        {
            if(i > 0)
            {
                if(s[i - 1] == '\\')
                    continue;
            }

            inChar = !inChar;
        }
    }

    return count;
}

int findLast(std::string s, const char *needle)
{
    int last = 0;
    bool inString = false, inChar = false;

    for(unsigned int i = 0; i < s.length(); ++i)
    {
        // Make sure it's not escaped
        if(i > 0)
        {
            if(s[i - 1] == '\\')
                continue;
        }

        // std::cout << "Looking for " << needle[0] << ". In char: " << (inChar ? "yes" : "no") << std::endl;

        if(s[i] == needle[0] && (needle[0] == '"' ? true : !inString) && (needle[0] == '\'' ? true : !inChar))
        {
            last = i;
        }

        if(s[i] == '"' && !inChar)
        {
            if(i > 0)
            {
                if(s[i - 1] == '\\')
                    continue;
            }

            inString = !inString;
        }

        if(s[i] == '\'' && !inString)
        {
            if(i > 0)
            {
                if(s[i - 1] == '\\')
                    continue;
            }

            inChar = !inChar;
        }
    }

    return last;
}

namespace text
{
    namespace TextUtils
    {
        std::string getClipboardData()
        {
            #ifdef WIN32

            HANDLE clip;
            if(OpenClipboard(0))
            {
                return std::string(GetClipboardData(CF_TEXT));
            }

            #else

            FILE* pipe = popen("xclip -o", "r");
            if (!pipe) return "";
            char buffer[4096];
            std::string result = "";
            while(!feof(pipe))
            {
                if(fgets(buffer, 4096, pipe) != NULL)
                    result += buffer;
            }
            pclose(pipe);

            return result;

            #endif

            return "";
        }

        bool replace(std::string& str, const std::string& from, const std::string& to)
        {
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

        bool applySyntaxHighlighting(std::string &s, std::vector<std::string> &keywords, bool multiline)
        {
            std::stringstream start;
            start << "`" << graphics::COLORSET_DEFAULT;
            if(multiline)
                start << "`" << graphics::COLORSET_COMMENT_START;


            // Highlight numbers
            for(unsigned int i = 0; i < s.length(); ++i)
            {
                // Check if we are between quotes
                std::string before = s.substr(0, i);
                // std::cout << "Before: '" << before.c_str() << "' at " << pos + deletedChars << std::endl;
                int quoteCount = countChars(before, "\"");
                if(quoteCount % 2 != 0)
                {
                    // std::cout << "Skipping at " << pos << "because of quotes" << std::endl;
                    ++i;
                    continue;
                }

                quoteCount = countChars(before, "'");
                if(quoteCount % 2 != 0)
                {
                    // std::cout << "Skipping at " << pos << "because of quotes" << std::endl;
                    ++i;
                    continue;
                }

                if(isdigit(s[i]) )
                {
                    if(i > 0 && alphanums.find(s[i - 1]) != std::string::npos && alphanums.find(s[i - 1]) != 0)
                        continue;

                    if(i > 0 && s[i - 1] == '`')
                        continue;

                    std::stringstream sub;
                    // Find next non-digit char
                    unsigned int len = 0;

                    for(unsigned int x = i + 1; x < s.length(); ++x)
                    {
                        if(x > 0 && s[x - 1] == '`')
                            continue;

                        if(!isxdigit(s[x]) && s[x] != 'x' && s[x] != '.')
                        {
                            sub.str("");
                            sub << "`" << graphics::COLORSET_DEFAULT;
                            s.insert(x, sub.str());
                            len = x - (i + 1);
                            break;
                        }
                    }

                    sub << "`" << graphics::COLORSET_NUMBERS;
                    s.insert(i, sub.str());
                    i += len + 6;
                }
            }

            // Highlight keywords
            for(unsigned int i = 0; i < keywords.size(); ++i)
            {
                // Try new method, much faster
                int pos = -1;

                while(s.find(keywords[i].c_str(), pos + 1) != std::string::npos)
                {
                    pos = s.find(keywords[i].c_str(), pos + 1);
                    unsigned int len = keywords[i].length();
                    if(pos > 0)
                    {
                        char needle[2];
                        needle[0] = s[pos - 1];
                        needle[1] = '\0';
                        if(alphanums.find(std::string(needle)) != std::string::npos)
                        {
                            ++pos;
                            continue;
                        }
                    }

                    if((unsigned int) pos < s.length())
                    {
                        char needle[2];
                        needle[0] = s[pos + len];
                        needle[1] = '\0';
                        if(alphanums.find(std::string(needle)) != std::string::npos && alphanums.find(std::string(needle)) != 0)
                        {
                            ++pos;
                            continue;
                        }
                    }

                    // Check if we are between quotes
                    std::string before = s.substr(0, pos);
                    // std::cout << "Before: '" << before.c_str() << "' at " << pos + deletedChars << std::endl;
                    int quoteCount = countChars(before, "\"");
                    if(quoteCount % 2 != 0)
                    {
                        // std::cout << "Skipping at " << pos << "because of quotes" << std::endl;
                        ++pos;
                        continue;
                    }

                    quoteCount = countChars(before, "'");
                    if(quoteCount % 2 != 0)
                    {
                        // std::cout << "Skipping at " << pos << "because of quotes" << std::endl;
                        ++pos;
                        continue;
                    }

                    std::stringstream sub;
                    sub << '`' << graphics::COLORSET_KEYWORD << keywords[i] << '`' << graphics::COLORSET_DEFAULT;
                    // std::cout << "Replacing '" << s.substr(pos, len).c_str() << "' at " << pos << " with '" << sub.str().c_str() << "'" << std::endl;
                    s.replace(pos, len, sub.str());

                    ++pos;
                }
            }

            int pos = -1;

            // Check for comments and preprocesser thingies
            bool singleline = false;
            for(unsigned int i = 0; i < s.length(); ++i)
            {
                // Make sure we're not in a string
                std::string before = s.substr(0, i);
                int quoteCount = countChars(before, "\"");
                if(quoteCount % 2 != 0)
                    continue;

                quoteCount = countChars(before, "'");
                if(quoteCount % 2 != 0)
                    continue;

                if(s[i] == '/' && i < s.length() - 1 && s[i + 1] == '/' && !multiline && !singleline)
                {
                    singleline = true;
                    std::stringstream sub;
                    sub << "`" << graphics::COLORSET_COMMENT_START;
                    s.insert(i, sub.str());
                }
                else if(s[i] == '/' && i < s.length() - 1 && s[i + 1] == '*' && !multiline && !singleline)
                {
                    multiline = true;
                    std::stringstream sub;
                    sub << "`" << graphics::COLORSET_COMMENT_START;
                    s.insert(i, sub.str());
                    i += 2;
                }
                else if(s[i] == '*' && i < s.length() - 1 && s[i + 1] == '/' && multiline && !singleline)
                {
                    multiline = false;
                    std::stringstream sub;
                    sub << "`" << graphics::COLORSET_COMMENT_END;
                    s.insert(i + 2, sub.str());
                    i += 2;
                }
                else if(s[i] == '#' && !multiline && !singleline)
                {
                    std::stringstream sub;
                    sub << "`" << graphics::COLORSET_PREPROC;
                    s.insert(i, sub.str());

                    i += 2;
                }
            }

            // Highlight strings
            while(s.find("\"", pos + 1) != std::string::npos)
            {
                pos = s.find("\"", pos + 1);

                // Make sure that it is not escaped
                if(pos > 0)
                {
                    if(s[pos - 1] == '\\')
                    {
                        // ++pos;
                        continue;
                    }
                }

                unsigned int len = 1;
                std::stringstream sub;

                // Make sure we're not in a char
                std::string before = s.substr(0, pos);

                int quoteCount = countChars(before, "'");

                if(quoteCount % 2 == 0)
                {
                    // Check if we are between quotes already
                    quoteCount = countChars(before, "\"");
                    if(quoteCount % 2 != 0) // Close
                    {
                        sub << "`" << graphics::COLORSET_DEFAULT;
                        s.insert(pos + 1, sub.str());
                        ++pos;
                    }
                    else // Open
                    {
                        sub << '`' << graphics::COLORSET_STRINGS << '"';
                        s.replace(pos, len, sub.str());
                        pos += 2;
                    }
                }
            }

            // Highlight chars
            pos = -1;
            while(s.find("'", pos + 1) != std::string::npos)
            {
                pos = s.find("'", pos + 1);

                // Make sure that it is not escaped
                if(pos > 0)
                {
                    if(s[pos - 1] == '\\')
                    {
                        // ++pos;
                        continue;
                    }
                }

                unsigned int len = 1;
                std::stringstream sub;

                std::string before = s.substr(0, pos);
                int quoteCount = countChars(before, "\"");

                if(quoteCount % 2 == 0)
                {
                    // Check if we are between quotes already
                    quoteCount = countChars(before, "'");
                    if(quoteCount % 2 != 0) // Close
                    {
                        sub << "`" << graphics::COLORSET_DEFAULT;
                        s.insert(pos + 1, sub.str());
                        ++pos;
                    }
                    else // Open
                    {
                        sub << '`' << graphics::COLORSET_CHARS << '\'';
                        s.replace(pos, len, sub.str());
                        pos += 2;
                    }
                }
            }

            // Highlight operators
            for(unsigned int i = 0; i < operators.length(); ++i)
            {
                // Try new method, much faster
                int pos = -1;

                char curOp[2];
                curOp[0] = operators[i];
                curOp[1] = '\0';

                while(s.find(curOp, pos + 1) != std::string::npos)
                {
                    pos = s.find(curOp, pos + 1);
                    unsigned int len = std::string(curOp).length();

                    // Check if we are between quotes
                    std::string before = s.substr(0, pos);
                    // std::cout << "Before: '" << before.c_str() << "' at " << pos + deletedChars << std::endl;
                    int quoteCount = countChars(before, "\"");
                    if(quoteCount % 2 != 0)
                    {
                        ++pos;
                        continue;
                    }

                    std::stringstream sub;
                    sub << '`' << graphics::COLORSET_OPS << curOp << '`' << graphics::COLORSET_DEFAULT;
                    s.replace(pos, len, sub.str());

                    pos += 2;
                }
            }

            s.insert(0, start.str());

            return multiline;
        }

        void highlightLines(std::vector<std::string> &lines, std::vector<std::string> &keywords)
        {
            bool multiline = false; // In a multiline comment?

            for(unsigned int i = 0; i < lines.size(); ++i)
            {
                multiline = applySyntaxHighlighting(lines[i], keywords, multiline);
            }
        }

        std::vector<EditorChar> getEditorCharVector(std::string &s, graphics::ColorScheme &curCS)
        {
            std::vector<EditorChar> chars;
            graphics::Color curBG = curCS.defaultBG, curFG = curCS.defaultFG;

            bool inComment = false, inPreproc = false;

            for(unsigned int i = 0; i < s.length(); ++i)
            {
                if(s[i] == '`' && isxdigit(s[i + 1]))
                {
                    int code;
                    char codeString[1];
                    codeString[0] = s[i + 1];
                    sscanf(codeString, "%i", &code);

                    if(!inComment)
                    {
                        if(!inPreproc)
                        {
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
                                case graphics::COLORSET_OPS:
                                    curBG = curCS.opsBG; curFG = curCS.opsFG;
                                    break;
                                case graphics::COLORSET_PREPROC:
                                    curBG = curCS.preprocBG; curFG = curCS.preprocFG;
                                    inPreproc = true;
                                    break;
                            }
                        }

                        if(code == graphics::COLORSET_COMMENT_START)
                        {
                            curBG = curCS.commentsBG; curFG = curCS.commentsFG;
                            inComment = true;
                        }
                    }
                    else
                    {
                        if(code == graphics::COLORSET_COMMENT_END)
                        {
                            inComment = false;
                            curBG = curCS.defaultBG; curFG = curCS.defaultFG;
                        }
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

            EditorChar c;
            c.bgColor.r = c.bgColor.g = c.bgColor.b = c.bgColor.a = 0;
            c.fgColor.r = c.fgColor.g = c.fgColor.b = c.fgColor.a = 0;
            c.content = '\0';
            chars.push_back(c);

            return chars;
        }

        void printEditorChars(std::vector<EditorChar> &chars)
        {
            for(unsigned int i = 0; i < chars.size(); ++i)
            {
                // std::cout << "Printing char with color " << chars[i].fgColor.term << std::endl;
                std::stringstream colorcode;
                colorcode << "\033[0;" << 30 + chars[i].fgColor.term << "m";

                std::cout << colorcode.str().c_str() << chars[i].content;
            }

            std::cout << std::endl;
        }
    }
}
