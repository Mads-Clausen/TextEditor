#include "graphics/TextEditorWindow.hpp"

#include <iomanip>
#include <fstream>
#include <sstream>

bool ctrlDown, altDown;

template <class T>
int numDigits(T number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

char* itoa(int val, int base)
{
	static char buf[32] = {0};

	int i = 30;

	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i + 1];
}

namespace graphics
{
    TextEditorWindow::~TextEditorWindow()
    {
        // Free the target
        if(_target != 0) SDL_FreeSurface(_target);

        // Delete all EditorChar pointers
        for(unsigned int y = 0; y < _lines.size(); ++y)
            delete _lines[y];
    }

    bool TextEditorWindow::init(int w, int h)
    {
        bool r = this->resize(w, h);
        _lines.push_back(new CharList); // Make sure we have one line to start with
        _lines[0]->push_back(' ');
        _tabLen = 4;

        _lang.path = "languages/c++";
        _lang.name = "C++";
        _lang.load();
        _lang.loadColors("colorschemes/default.csch");

        _saveFile = "save.txt";

        // Init the default font
        _fontSize = 14;
        _font = TTF_OpenFont("fonts/default.ttf", _fontSize);
        _lineNumFont = TTF_OpenFont("fonts/__line_nums.ttf", _fontSize);
        _spacing = 1;
        graphics::FontRenderer::setFont(_font);

        int x;
        TTF_SizeText(_font, "1", &x, &_fontHeight);

        _hlLines = this->getLines();
        highlightLines(_hlLines, _lang.keywords);

        this->render();

        return r;
    }

    void TextEditorWindow::resizeFont(int size)
    {
        TTF_CloseFont(_font);
        _fontSize = size;
        _font = TTF_OpenFont("fonts/default.ttf", _fontSize);
        _lineNumFont = TTF_OpenFont("fonts/__line_nums.ttf", _fontSize);

        graphics::FontRenderer::setFont(_font);

        int x;
        TTF_SizeText(_font, "1", &x, &_fontHeight);
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


        // std::cout << "Initialised TextEditorWindow with size " << w << "*" << h << std::endl;

        return true;
    }

    void TextEditorWindow::moveCursorDown()
    {
        // We can't really move down if we're at the bottom, eh?
        if(_cursorY < _lines.size() - 1)
        {
            ++_cursorY; // Move down

            if((_cursorY - _scrollY) >= (int) _target->h / _fontHeight && _scrollY < (int) _lines.size())
            {
                // std::cout << "Scrolling. " << (int) _target->h / _fontHeight << std::endl;
                ++_scrollY;
            }

            // Make sure that we're not trying to access non-existing chars
            if(_cursorX >= _lines[_cursorY]->size())
                _cursorX = _lines[_cursorY]->size();
        }

        // std::cout << _cursorY << " & " << _scrollY << std::endl;
    }

    void TextEditorWindow::moveCursorUp()
    {
        if(_cursorY > 0)
        {
            --_cursorY;

            if(_scrollY > 0)
                --_scrollY;

            // Make sure that we're not trying to access non-existing chars
            if(_cursorX >= _lines[_cursorY]->size())
                _cursorX = _lines[_cursorY]->size();
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
                _cursorX = _lines[_cursorY]->size();
        }
    }

    void TextEditorWindow::addChar(char c)
    {
        // std::cout << "Adding char '" << c << "' at (" << _cursorX << ", " << _cursorY << ")" << std::endl;
        _lines[_cursorY]->insert(_lines[_cursorY]->begin() + _cursorX, c); // Insert the char

        // std::cout << "Char at (" << _cursorX << ", " << _cursorY << "): '" << (*(_lines[_cursorY]))[_cursorX] << "'" << std::endl;
        this->moveCursorRight(); // Go to the right
    }

    void TextEditorWindow::removeChar()
    {
        if(_cursorX > 0)
        {
            // std::cout << "Removing char at (" << (_cursorX - 1) << ", " << _cursorY << ")" << std::endl;
            _lines[_cursorY]->erase(_lines[_cursorY]->begin() + (_cursorX - 1));
            this->moveCursorLeft();
        }
        else
        {
            // Make sure we're not moving stuff to line -1
            if(_cursorY > 0)
            {
                unsigned int oldSize = _lines[_cursorY - 1]->size();

                // Move current line above
                for(unsigned int x = 0; x < _lines[_cursorY]->size(); ++x)
                    // We can push since we're appending to the end of the line
                    _lines[_cursorY - 1]->push_back((*(_lines[_cursorY]))[x]);

                // Remove old line
                _lines.erase(_lines.begin() + _cursorY);

                this->moveCursorUp();
                _cursorX = oldSize;
            }
        }
    }

    void TextEditorWindow::insertText(std::string s)
    {
        std::stringstream ss;
        for(unsigned int i = 0; i < _tabLen; ++i)
            ss << ' ';

        replace_str(s, "\t", ss.str());
        for(unsigned int i = 0; i < s.length(); ++i)
        {
            if(s[i] == '\n')
            {
                this->addLine();
                continue;
            }

            this->addChar(s[i]);
        }
    }

    void TextEditorWindow::addLine()
    {
        _lines.insert(_lines.begin() + (_cursorY + 1), new CharList());

        // Move the rest of the current line to the one below
        ++_cursorX;
        for(unsigned int x = _cursorX - 1; x < _lines[_cursorY]->size(); ++x)
            _lines[_cursorY + 1]->push_back((*(_lines[_cursorY]))[x]);

        for(unsigned int x = _cursorX - 1; x < _lines[_cursorY]->size();) // No need to increment x, as we are deleting chars
            _lines[_cursorY]->erase(_lines[_cursorY]->begin() + x);

        this->moveCursorDown();

        _cursorX = 0;
    }

    void TextEditorWindow::scroll(bool dir)
    {
        // True = up, false = down
        if(dir && _scrollY >= 3)
            _scrollY -= 3;
        else if(dir && _scrollY < 3)
            _scrollY = 0;
        else if(!dir && _scrollY < _lines.size() - (_target->h / _fontHeight + 3) && _lines.size() >= _target->h / _fontHeight + 3)
        {
            _scrollY += 3;
        }
        // std::cout << "Scrolling" << std::endl;
    }

    void TextEditorWindow::attemptMoveCursor(int x, int y)
    {
        int cY = y / (_fontHeight - 2) + _scrollY, cX = 0;

        if(cY >= _lines.size())
            cY = _lines.size() - 1;

        int last = 0;
        bool found = false;
        for(unsigned int i = 0; i < _lines[cY]->size(); ++i)
        {
            int xcpy = x;
            std::string str;
            for(unsigned int i2 = 0; i2 < i; ++i2)
            {
                std::stringstream ss;
                ss << (*(_lines[cY]))[i2];
                str.append(ss.str());
            }

            int newX, newY;
            TTF_SizeText(_font, str.c_str(), &newX, &newY);

            xcpy -= (numDigits(_lines.size() + 1) * 7 + 4 + 2) + i * _spacing;
            if(xcpy >= last - _spacing && xcpy < newX)
            {
                cX = i - 1;

                found = true;
                break;
            }

            last = newX;
        }

        _cursorX = (found ? cX : (x - (numDigits(_lines.size() + 1) * 7 + 4 + 2) <= 0 ? 0 : _lines[cY]->size()));
        _cursorY = cY;
        // std::cout << "Moved cursor to (" << _cursorX << ", " << _cursorY << ")" << std::endl;
    }

    void TextEditorWindow::onMouseEvent(SDL_MouseButtonEvent &ev, bool dir)
    {
        if(!_active)
            return;

        switch(ev.button)
        {
            case SDL_BUTTON_WHEELDOWN:
                if(ctrlDown)
                    this->resizeFont(_fontSize + 1);
                else
                    this->scroll(false);

                break;
            case SDL_BUTTON_WHEELUP:
                if(ctrlDown)
                    this->resizeFont(_fontSize - 1);
                else
                    this->scroll(true);

                break;
            case SDL_BUTTON_LEFT:
                this->attemptMoveCursor(ev.x - _posX, ev.y - _posY);
                break;
            default:
                break;
        }


        _hlLines = this->getLines();
        highlightLines(_hlLines, _lang.keywords, _scrollY, _scrollY + _target->h / _fontHeight + 5);
        this->render();
    }

    void TextEditorWindow::onKeyEvent(SDL_KeyboardEvent &key, bool dir)
    {
        if(!_active)
            return;

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
                this->moveCursorUp();
            }
            else if(key.keysym.sym == SDLK_BACKSPACE)
            {
                this->removeChar();
            }
            else if(key.keysym.sym == SDLK_RETURN)
            {
                this->addLine();
            }
            else if(key.keysym.sym == SDLK_TAB)
            {
                for(unsigned int i = 0; i < _tabLen; ++i)
                    addChar(' ');
            }
            else if(key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
            {
                ctrlDown = true;
            }
            else if(key.keysym.sym == SDLK_s && ctrlDown)
            {
                this->save();
            }
            else if(key.keysym.sym == SDLK_v && ctrlDown)
            {
                this->insertText(text::TextUtils::getClipboardData());
            }
            else if(key.keysym.sym == SDLK_F1)
            {
                _debug = !_debug;
            }
            else if(key.keysym.sym == SDLK_LSHIFT || key.keysym.sym == SDLK_RSHIFT || key.keysym.sym == SDLK_RALT || key.keysym.sym == SDLK_LALT || key.keysym.sym == SDLK_CAPSLOCK) {}
            else
            {
                // Get the unicode char
                if(!ctrlDown && !altDown && kName < 0x80 && kName > 0)
                {
                    this->addChar(kName);
                }
                else
                {
                }
            }

            _hlLines = this->getLines();
            highlightLines(_hlLines, _lang.keywords, _scrollY, _scrollY + _target->h / _fontHeight + 5);
            this->render();
        }
        else // UP
        {
            // std::cout << "Key " << (int) key.keysym.sym << " AKA '" << kName.c_str() << "' was released." << std::endl;
            if(key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
            {
                ctrlDown = false;
            }
        }
    }

    void TextEditorWindow::save()
    {
        std::cout << "Saving" << std::endl;
        std::ofstream file(_saveFile.c_str());

        for(unsigned int y = 0; y < _lines.size(); ++y)
        {
            for(unsigned int x = 0; x < _lines[y]->size(); ++x)
            {
                file << (*(_lines[y]))[x];
            }

            file << std::endl;
        }

        file.close();
    }

    std::vector<std::string> TextEditorWindow::getLines()
    {
        std::vector<std::string> l;
        for(unsigned int y = 0; y < _lines.size(); ++y)
        {
            char line[_lines[y]->size() + 1];

            for(unsigned int x = 0; x < _lines[y]->size(); ++x)
            {
                line[x] = (*(_lines[y]))[x];
            }

            line[_lines[y]->size()] = '\0';
            l.push_back(line);
        }

        return l;
    }

    void TextEditorWindow::update()
    {
        if(!_active)
            return;

        ++_caretWait;
        if(_caretWait > 80)
        {
            _caretVisible = !_caretVisible;
            _caretWait = 0;
        }

        _caretVisible = true;
    }

    void TextEditorWindow::render()
    {
        if(!_active)
            return;

        SDL_FillRect(_target, 0, SDL_MapRGB(_target->format, _lang.colorScheme.defaultBG.r, _lang.colorScheme.defaultBG.g, _lang.colorScheme.defaultBG.b));
        std::vector<std::string> &lines = _hlLines;

        graphics::FontRenderer::setTarget(_target);
        graphics::FontRenderer::setFont(_font);

        int lnDigits = numDigits(_lines.size() + 1);
        char *lineNumString = itoa(_lines.size(), 10);

        SDL_Rect lnRectDst;
        lnRectDst.x = lnRectDst.y = 0;
        int x, _unusedY;
        TTF_SizeText(_lineNumFont, lineNumString, &x, &_unusedY);
        lnRectDst.w = x + 6;
        lnRectDst.h = (_lines.size() * _fontSize < _target->h ? _target->h : _lines.size() * _fontSize);
        SDL_FillRect(_target, &lnRectDst, SDL_MapRGB(_target->format, _lang.colorScheme.defaultBG.r - 20, _lang.colorScheme.defaultBG.g - 20, _lang.colorScheme.defaultBG.b - 20));

        for(unsigned int y = 0; y < lines.size(); ++y)
        {
            std::vector<text::EditorChar> curLine = getEditorCharVector(lines[y], _lang.colorScheme);
            for(unsigned int x = 0; x < curLine.size(); ++x)
            {
                int lX, lY;

                char line[x + 1];
                for(unsigned int i = 0; i < x; ++i)
                {
                    line[i] = (*(_lines[y]))[i];
                }
                line[x] = '\0';

                TTF_SizeText(_font, line, &lX, &lY);
                // std::cout << lY << std::endl;

                if(x == 0)
                {
                    char *l = itoa(y + 1, 10);

                    graphics::Color c(_lang.colorScheme.defaultFG.r + 40, _lang.colorScheme.defaultFG.g + 40, _lang.colorScheme.defaultFG.b + 40, 255);
                    graphics::Color bgC(_lang.colorScheme.defaultBG.r - 20, _lang.colorScheme.defaultBG.g - 20, _lang.colorScheme.defaultBG.b - 20, 255);
                    for(unsigned int i = 0; i < lnDigits; ++i)
                    {
                        graphics::FontRenderer::setFont(_lineNumFont);
                        int w, __h;
                        TTF_SizeText(_lineNumFont, std::string(lineNumString).substr(0, i).c_str(), &w, &__h);
                        graphics::FontRenderer::renderLetter(l[i], w + 2, (y - _scrollY) * (lY - 2), c, bgC);
                        graphics::FontRenderer::setFont(_font);
                    }
                }

                graphics::FontRenderer::renderLetter(curLine[x].content, lX + x * _spacing + 2 + lnRectDst.w, (y - _scrollY) * (lY - 2), curLine[x].fgColor, curLine[x].bgColor);
            }
        }

        // Draw the caret
        if(_caretVisible)
        {
            int lX, lY;

            char line[_cursorX + 1];
            for(unsigned int i = 0; i < _cursorX; ++i)
            {
                line[i] = (*(_lines[_cursorY]))[i];
            }
            line[_cursorX] = '\0';

            TTF_SizeText(_font, line, &lX, &lY);

            if(_debug)
            {
                // Draw some reference lines
                graphics::Color col(255, 0, 0, 255);
                graphics::line(_target, 0, (_cursorY - _scrollY) * (_fontHeight - 2), _target->w, (_cursorY - _scrollY) * (_fontHeight - 2), col);

                col.r = 0;
                col.b = 255;
                graphics::line(_target, 0, (_cursorY - _scrollY) * (_fontHeight - 2) + (_fontHeight - 2), _target->w, (_cursorY - _scrollY) * (_fontHeight - 2) + (_fontHeight - 2), col);

                col.b = 0;
                col.g = 255;
                graphics::line(_target, lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, 0, lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, _target->h, col);
            }

            // std::cout << "Drawing caret at y " << (_cursorY - _scrollY) * (_fontHeight - 2) << " vs " << (_cursorY - _scrollY) * (_fontHeight - 2) + (_fontHeight - 2) << std::endl;
            graphics::line(_target, lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 2), lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 2) + (_fontHeight - 2), _lang.colorScheme.caretFG);
        }

        SDL_Rect pos;
        pos.x = _posX;
        pos.y = _posY;
        SDL_BlitSurface(_target, 0, _screen, &pos);
        GraphicsManager::flip();
    }
}
