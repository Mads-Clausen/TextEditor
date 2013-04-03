/**
 * @file Has various classes and functions to help with text manipulation.
 */

#ifndef TEXTUTILS_INCLUDED
#define TEXTUTILS_INCLUDED

#include <string>
#include <vector>

#include "text/EditorChar.hpp"

namespace text
{
    namespace TextUtils
    {
        /**
         * Makes a single character lowercase.
         *
         * @param c The character.
         */
        void toLowercase(char &c);

        /**
         * Makes a single character uppercase.
         *
         * @param c The character.
         */
        void toUppercase(char &c);

        /**
         * Makes a whole string lowercase.
         *
         * @param s The std::string.
         */
        void toLowercase(std::string &s);

        /**
         * Makes a whole string uppercase.
         *
         * @param s The std::string.
         */
        void toUppercase(std::string &s);

        /**
         * Applies syntax highlighting to a string of text. Uses the current colourscheme.
         *
         * @param s The string.
         * @param keywords The keywords(for, if, while, etc.)
         */
        void applySyntaxHighlighting(std::string &s, std::vector<const char*> keywords);

        /**
         * Replaces all occurences of a substring with another one.
         *
         * @param str The "main" string.
         * @param from The substring to be replaced.
         * @param to The substring to replace with.
         *
         * @return Success?
         */
        bool replace(std::string& str, const std::string& from, const std::string& to);

        /**
         * Translates a string into a vector of EditorChars.
         *
         * @param s The string.
         *
         * @return A vector with the EditorChars
         */
        std::vector<EditorChar> getEditorCharVector(std::string &s);

        /**
         * Prints the EditorChars to the console.
         *
         * @param chars The vector with EditorChars.
         */
        void printEditorChars(std::vector<EditorChar> &chars);

        /**
         * Sets the current colorscheme.
         *
         * @param sc The new scheme.
         */
        void setColorScheme(graphics::ColorScheme sc);
    }
}
#endif // TEXT_INCLUDED
