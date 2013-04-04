#include "graphics/TextEditorWindow.hpp"

TextEditorWindow::~TextEditorWindow()
{
    // Free the target
    if(_target != 0) SDL_FreeSurface(_target);

    // Delete all EditorChar pointers
    for(unsigned int y = 0; y < _lines.size(); ++y)
    {
        delete _lines[y];
    }
}

bool TextEditorWindow::init(int w, int h)
{
    bool r = this->resize(w, h);
    std::cout << "Initialised TextEditorWindow with size " << w << "*" << h << std::endl;
    _lines.push_back(new CharList); // Make sure we have one line to start with

    return r;
}

bool TextEditorWindow::resize(int w, int h)
{
    // Free the target to avoid memory leaks
    if(_target != 0) SDL_FreeSurface(_target);

    // Create new target, making sure to use the right architecture
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        _target = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    #else
        _target = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    #endif

    // Something went wrong
    if(_target == 0)
    {
        std::cerr << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void TextEditorWindow::moveCursorDown()
{
    // We can't really move down if we're at the bottom, eh?
    if(_cursorY < _lines.size() - 1)
    {
        ++_cursorY; // Move down

        // Make sure that we're not trying to access non-existing chars
        if(_cursorX >= _lines[_cursorY]->size())
            _cursorX = _lines[_cursorY]->size() - 1;
    }
}

void TextEditorWindow::moveCursorUp()
{
    if(_cursorY > 0)
    {
        --_cursorY;

        // Make sure that we're not trying to access non-existing chars
        if(_cursorX >= _lines[_cursorY]->size())
            _cursorX = _lines[_cursorY]->size() - 1;
    }
}

void TextEditorWindow::moveCursorRight()
{
    // Check that we're not moving further than the line is long... what?
    if(_cursorX < _lines[_cursorY]->size())
        ++_cursorX;
    else
        // Attempt to move down if the end of the line is reached
        this->moveCursorDown();
}

void TextEditorWindow::moveCursorLeft()
{
    // Check if we're trying to move into a wall
    if(_cursorX > 0)
        --_cursorX;
    else
    {   // If we are, attempt to move up
        unsigned int oldCY = _cursorY;
        this->moveCursorUp();

        // Move to the end of the line if the up-move was successful
        if(_cursorY != oldCY)
            _cursorX = _lines[_cursorY]->size() - 1;
    }
}

void TextEditorWindow::addChar(char c)
{
    std::cout << "Adding char '" << c << "' at (" << _cursorX << ", " << _cursorY << ")" << std::endl;
    _lines[_cursorY]->insert(_lines[_cursorY]->begin() + _cursorX, c); // Insert the char

    std::cout << "Char at (" << _cursorX << ", " << _cursorY << "): '" << (*(_lines[_cursorY]))[_cursorX] << "'" << std::endl;
    this->moveCursorRight(); // Go to the right
}

void TextEditorWindow::removeChar()
{
    if(_cursorX > 0)
        _lines[_cursorY]->erase(_lines[_cursorY]->begin() + (_cursorX - 1));
    else
    {
        // Make sure we're not moving stuff to line -1
        if(_cursorY > 0)
        {
            // Move current line above
            for(unsigned int x = 0; x < _lines[_cursorY]->size(); ++x)
            {
                // We can push since we're appending to the end of the line
                _lines[_cursorY - 1]->push_back((*(_lines[_cursorY]))[x]);
            }

            // Remove old line
            _lines.erase(_lines.begin() + _cursorY);
        }
    }
}

void TextEditorWindow::onKeyEvent(SDL_KeyboardEvent &key, bool dir)
{
    // Toggle capslock
    if(key.keysym.sym == SDLK_CAPSLOCK)
    {
        _capslock = !_capslock;
        return;
    }

    // Get the name of the key
    char kName = (char) key.keysym.unicode;
    /*
    if(_capslock)
        text::TextUtils::toUppercase(kName);
    //*/

    if(dir) // DOWN
    {
        // std::cout << "Key " << (int) key.keysym.sym << " AKA '" << kName.c_str() << "' was pressed." << std::endl;

        // Get the unicode char
        if(kName < 0x80 && kName > 0)
        {
            this->addChar(kName);
            // Print the entire line
            for(unsigned int x = 0; x < _lines[_cursorY]->size(); ++x)
            {
                std::cout << (*(_lines[_cursorY]))[x];
            }
            std::cout << std::endl;
        }
        else
        {
            // Unable to find character name
            /* ========================== */
            /*            TODO            */
            /* ========================== */
        }

        if(key.keysym.sym == SDLK_DOWN)
        {
            this->moveCursorDown();
        }
        else if(key.keysym.sym == SDLK_LEFT)
        {
            this->moveCursorLeft();
        }
        else if(key.keysym.sym == SDLK_RIGHT)
        {
            this->moveCursorRight();
        }
        else if(key.keysym.sym == SDLK_UP)
        {
            this->moveCursorDown();
        }
    }
    else // UP
    {
        // std::cout << "Key " << (int) key.keysym.sym << " AKA '" << kName.c_str() << "' was released." << std::endl;
    }
}
