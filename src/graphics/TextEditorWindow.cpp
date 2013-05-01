#include "graphics/TextEditorWindow.hpp"

#include <iomanip>
#include <fstream>
#include <sstream>

bool ctrlDown, altDown, shiftDown;

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

bool inSelection(int x, int y, Selection &sel)
{
    if(y > sel.start.y && y < sel.end.y)
        return true;

    if(y == sel.start.y && x > sel.start.x)
    {
        if(y == sel.end.y && x >= sel.end.x)
            return false;

        return true;
    }

    if(y == sel.end.y && x <= sel.end.x)
    {
        if(y == sel.start.y && x <= sel.start.x)
            return false;

        return true;
    }

    return false;
}

namespace graphics
{
    TextEditorWindow::~TextEditorWindow()
    {
        std::cout << "Shutting down TextEditorWindow" << std::endl;

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

        _curFile = "save.txt";

        // Init the default font
        _fontSize = 14;
        _font = TTF_OpenFont("fonts/default.ttf", _fontSize);
        _lineNumFont = TTF_OpenFont("fonts/__line_nums.ttf", _fontSize);
        _spacing = 1;
        graphics::FontRenderer::setFont(_font);

        TTF_SizeText(_font, "1", &_fontWidth, &_fontHeight);

        _hlLines = this->getLines();
        highlightLines(_hlLines, _lang.keywords);

        _cursors.push_back(Cursor(0, 0));

        this->setActive(true);
        this->render();
        this->setActive(false);

        return r;
    }

    void TextEditorWindow::resizeFont(int size)
    {
        TTF_CloseFont(_font);
        _fontSize = size;
        _font = TTF_OpenFont("fonts/default.ttf", _fontSize);
        _lineNumFont = TTF_OpenFont("fonts/__line_nums.ttf", _fontSize);

        graphics::FontRenderer::setFont(_font);

        TTF_SizeText(_font, "1", &_fontWidth, &_fontHeight);
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

    void TextEditorWindow::moveCursorDown(Cursor *cur)
    {
        int &_cursorX = cur->x, &_cursorY = cur->y;

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

            if((_cursorX - _scrollX) < 18)
                _scrollX = 0;
        }
    }

    void TextEditorWindow::moveCursorUp(Cursor *cur)
    {
        int &_cursorX = cur->x, &_cursorY = cur->y;

        if(_cursorY > 0)
        {
            --_cursorY;

            if(_scrollY > 0)
                --_scrollY;

            // Make sure that we're not trying to access non-existing chars
            if(_cursorX >= _lines[_cursorY]->size())
                _cursorX = _lines[_cursorY]->size();

            if((_cursorX - _scrollX) < 18)
                _scrollX = 0;
        }
    }

    void TextEditorWindow::moveCursorRight(Cursor *cur)
    {
        int &_cursorX = cur->x, &_cursorY = cur->y;

        // Check that we're not moving further than the line is long... what?
        if(_cursorX < _lines[_cursorY]->size())
        {
            ++_cursorX;

            if((_cursorX - _scrollX) >= _target->w / _fontWidth - 12 && _scrollX < _lines[_cursorY]->size())
                ++_scrollX;
        }
        else
            // Attempt to move down if the end of the line is reached
            this->moveCursorDown(cur);
    }

    void TextEditorWindow::moveCursorLeft(Cursor *cur)
    {
        int &_cursorX = cur->x, &_cursorY = cur->y;

        // Check if we're trying to move into a wall
        if(_cursorX > 0)
        {
            --_cursorX;

            if((_cursorX - _scrollX) < 6 && _scrollX > 0)
                --_scrollX;
        }
        else
        {   // If we are, attempt to move up
            unsigned int oldCY = _cursorY;
            this->moveCursorUp(cur);

            // Move to the end of the line if the up-move was successful
            if(_cursorY != oldCY)
                _cursorX = _lines[_cursorY]->size();
        }
    }

    void TextEditorWindow::addChar(char c)
    {
        for(unsigned int i = 0; i < _selections.size(); ++i)
        {
            Selection &sel = _selections[i];

            for(int y = _lines.size() - 1; y >= 0; --y)
            {
                for(int x = _lines[y]->size() - 1; x >= 0; --x)
                {
                    if(inSelection(x, y, sel))
                    {
                        _lines[y]->erase(_lines[y]->begin() + x);
                        if(_lines[y]->size() <= 0)
                        {
                            _lines.erase(_lines.begin() + y);
                        }
                    }
                }
            }
        }

        _selections.clear();

        for(unsigned int i = 0; i < _cursors.size(); ++i)
        {
            int &_cursorX = _cursors[i].x, &_cursorY = _cursors[i].y;

            _lines[_cursorY]->insert(_lines[_cursorY]->begin() + _cursorX, c); // Insert the char

            this->moveCursorRight(&_cursors[i]);
        }
    }

    void TextEditorWindow::removeChar()
    {
        std::vector<int> skips;

        for(unsigned int i = 0; i < _selections.size(); ++i)
        {
            Selection &sel = _selections[i];

            for(int y = _lines.size() - 1; y >= 0; --y)
            {
                for(int x = _lines[y]->size() - 1; x >= 0; --x)
                {
                    if(inSelection(x, y, sel))
                    {
                        _lines[y]->erase(_lines[y]->begin() + x);


                        if(_lines[y]->size() <= 0)
                            _lines.erase(_lines.begin() + y);
                    }
                }
            }

            if(sel.end.y > sel.start.y)
            {
                // Move current line above
                for(unsigned int x = 0; x < _lines[sel.start.y + 1]->size(); ++x)
                    // We can push since we're appending to the end of the line
                    _lines[sel.start.y]->push_back((*(_lines[sel.start.y + 1]))[x]);

                // Remove old line
                _lines.erase(_lines.begin() + sel.start.y + 1);
            }

            _cursors[i].x = sel.start.x;
            _cursors[i].y = sel.start.y;
            skips.push_back(i);
        }

        _selections.clear();

        for(unsigned int i = 0; i < _cursors.size(); ++i)
        {
            bool skip = false;
            for(unsigned int x = 0; x < skips.size(); ++x)
            {
                if(i == skips[x])
                {
                    skip = true;
                    break;
                }

            }

            if(skip) continue;

            int &_cursorX = _cursors[i].x, &_cursorY = _cursors[i].y;

            if(_cursorX > 0)
            {
                // std::cout << "Removing char at (" << (_cursorX - 1) << ", " << _cursorY << ")" << std::endl;
                _lines[_cursorY]->erase(_lines[_cursorY]->begin() + (_cursorX - 1));
                this->moveCursorLeft(&_cursors[i]);
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

                    this->moveCursorUp(&_cursors[i]);
                    _cursorX = oldSize;
                }
            }
        }
    }

    void TextEditorWindow::insertText(std::string s)
    {
        this->addChar('a');
        this->removeChar();

        _selections.clear();

        for(unsigned int i = 0; i < _cursors.size(); ++i)
        {
            int &_cursorX = _cursors[i].x, &_cursorY = _cursors[i].y;

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
    }

    void TextEditorWindow::addLine()
    {
        for(unsigned int i = 0; i < _cursors.size(); ++i)
        {
            int &_cursorX = _cursors[i].x, &_cursorY = _cursors[i].y;

            _lines.insert(_lines.begin() + (_cursorY + 1), new CharList());

            // Move the rest of the current line to the one below
            ++_cursorX;
            for(unsigned int x = _cursorX - 1; x < _lines[_cursorY]->size(); ++x)
                _lines[_cursorY + 1]->push_back((*(_lines[_cursorY]))[x]);

            for(unsigned int x = _cursorX - 1; x < _lines[_cursorY]->size();) // No need to increment x, as we are deleting chars
                _lines[_cursorY]->erase(_lines[_cursorY]->begin() + x);

            this->moveCursorDown(&_cursors[i]);

            _cursorX = 0;
        }

        _scrollX = 0;
    }

    void TextEditorWindow::scroll(bool dir)
    {
        // True = up, false = down
        if(dir && _scrollY >= 3)
            _scrollY -= 3;
        else if(dir && _scrollY < 3)
            _scrollY = 0;
        else if(!dir && _scrollY < _lines.size() - (_target->h / _fontHeight) && _lines.size() >= _target->h / _fontHeight + 3)
        {
            _scrollY += 3;
        }
        // std::cout << "Scrolling" << std::endl;
    }

    void TextEditorWindow::attemptMoveCursor(Cursor *c, int x, int y)
    {
        int cY = y / (_fontHeight - 1) + _scrollY, cX = 0;

        if(cY >= _lines.size())
            cY = _lines.size() - 1;

        int scrollW, __y;
        char line[_scrollX + 1];
        for(unsigned int i = 0; i < _scrollX; ++i)
        {
            line[i] = (*(_lines[cY]))[i];
        }
        line[_scrollX] = '\0';
        TTF_SizeText(_font, line, &scrollW, &__y);

        int last = 0;
        bool found = false;
        for(unsigned int i = 0; i < _lines[cY]->size(); ++i)
        {
            int xcpy = x + scrollW + (_scrollX) * _spacing;
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

        c->x = (found ? cX : (x - (numDigits(_lines.size() + 1) * 7 + 4 + 2) <= 0 ? 0 : _lines[cY]->size()));
        c->y = cY;
        // std::cout << "Moved cursor to (" << _cursorX << ", " << _cursorY << ")" << std::endl;
    }

    int lastX, lastY;
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
                if(dir)
                {
                    if(!ctrlDown && !shiftDown)
                    {
                        _selections.clear();

                        Cursor c;
                        this->attemptMoveCursor(&c, ev.x - _posX, ev.y - _posY);
                        lastX = c.x; lastY = c.y;
                        _cursors.clear();
                        _cursors.push_back(c);
                        this->attemptMoveCursor(&(_cursors[0]), ev.x - _posX, ev.y - _posY);

                        if(_cursors[0].x - _scrollX < 6)
                            _scrollX -= 6;

                        if(_scrollX < 0)
                            _scrollX = 0;

                        if((_cursors[0].x - _scrollX) >= _target->w / _fontWidth - 12 && _scrollX < _lines[_cursors[0].y]->size())
                            ++_scrollX;

                        if(_scrollX > _lines[_cursors[0].y]->size() - 15)
                            _scrollX = _lines[_cursors[0].y]->size() - 15;
                    }
                    else if(shiftDown)
                    {
                        //*
                        _selections.clear();

                        Cursor c;
                        this->attemptMoveCursor(&c, ev.x - _posX, ev.y - _posY);
                        Selection sel;
                        _selections.push_back(sel);
                        _selections[0].start = _cursors[_cursors.size() - 1];
                        _selections[0].end = c;

                        // Swap them if start is greater than end
                        if((_selections[0].start.y > _selections[0].end.y) || (_selections[0].start.y == _selections[0].end.y && _selections[0].start.x > _selections[0].end.x))
                        {
                            Cursor _s = _selections[0].start;
                            _selections[0].start = _selections[0].end;
                            _selections[0].end = _s;
                        }

                        std::cout << "Selection from (" << _selections[0].start.x << ", " << _selections[0].start.y << ") to (" << _selections[0].end.x << ", " << _selections[0].end.y << ")" << std::endl;
                        //*/
                    }
                    else if(ctrlDown)
                    {
                        _selections.clear();

                        Cursor c;
                        this->attemptMoveCursor(&c, ev.x - _posX, ev.y - _posY);
                        _cursors.push_back(c);
                    }
                    else
                    {
                        _selections.clear();

                        Cursor c;
                        this->attemptMoveCursor(&c, ev.x - _posX, ev.y - _posY);
                        Selection sel;
                        sel.start = c;
                        _selections.push_back(sel);
                        this->attemptMoveCursor(&(_cursors[_cursors.size() - 1]), ev.x - _posX, ev.y - _posY);
                    }
                }
                else
                {
                    if(!shiftDown && !ctrlDown)
                    {
                        _cursors.clear();
                        Cursor c;
                        this->attemptMoveCursor(&c, ev.x - _posX, ev.y - _posY);

                        if(lastX != c.x || lastY != c.y)
                        {
                            Selection sel;
                            sel.start.x = lastX - 1;
                            sel.start.y = lastY;
                            sel.end = c;
                            _selections.push_back(sel);
                        }
                        else
                            _selections.clear();

                        _cursors.push_back(c);
                    }
                }

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
                _selections.clear();

                int cX = _cursors[_cursors.size() - 1].x, cY = _cursors[_cursors.size() - 1].y;
                _cursors.clear();
                _cursors.push_back(Cursor(cX, cY));
                this->moveCursorDown(&_cursors[0]);
            }
            else if(key.keysym.sym == SDLK_LEFT)
            {
                _selections.clear();

                int cX = _cursors[_cursors.size() - 1].x, cY = _cursors[_cursors.size() - 1].y;
                _cursors.clear();
                _cursors.push_back(Cursor(cX, cY));
                this->moveCursorLeft(&_cursors[0]);
            }
            else if(key.keysym.sym == SDLK_RIGHT)
            {
                _selections.clear();

                int cX = _cursors[_cursors.size() - 1].x, cY = _cursors[_cursors.size() - 1].y;
                _cursors.clear();
                _cursors.push_back(Cursor(cX, cY));
                this->moveCursorRight(&_cursors[0]);
            }
            else if(key.keysym.sym == SDLK_UP)
            {
                _selections.clear();

                int cX = _cursors[_cursors.size() - 1].x, cY = _cursors[_cursors.size() - 1].y;
                _cursors.clear();
                _cursors.push_back(Cursor(cX, cY));
                this->moveCursorUp(&_cursors[0]);
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
            else if(key.keysym.sym == SDLK_LSHIFT || key.keysym.sym == SDLK_RSHIFT)
            {
                shiftDown = true;
            }
            else if(key.keysym.sym == SDLK_RALT || key.keysym.sym == SDLK_LALT)
            {
                altDown = true;
            }
            else if(key.keysym.sym == SDLK_CAPSLOCK) {}
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
            else if(key.keysym.sym == SDLK_LSHIFT || key.keysym.sym == SDLK_RSHIFT)
            {
                shiftDown = false;
            }
            else if(key.keysym.sym == SDLK_LALT || key.keysym.sym == SDLK_RALT)
            {
                altDown = false;
            }
        }
    }

    void TextEditorWindow::save()
    {
        std::cout << "Saving" << std::endl;
        std::ofstream file(_curFile.c_str());

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

    void TextEditorWindow::load(char *path)
    {
        _lines.clear();
        _cursors.clear();
        _selections.clear();
        _cursors.push_back(Cursor(0, 0));

        std::cout << "Loading file '" << path << "'" << std::endl;
        _curFile = std::string(path);

        std::ifstream file(_curFile.c_str());

        if(!file.is_open())
        {
            std::cout << "Unable to load file '" << _curFile.c_str() << "'";
            _lines.push_back(new CharList);
            return;
        }

        int y = 0;
        for(std::string line; getline(file, line);)
        {
            _lines.push_back(new CharList);

            for(unsigned int i = 0; i < line.length(); ++i)
                _lines[_lines.size() - 1]->push_back(line[i]);
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

        for(unsigned int i = 0; i < _cursors.size(); ++i)
        {
            if(_cursors[i].y >= _lines.size())
                _cursors[i].y = _lines.size() - 1;
        }

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
            int lX, lY;
            char line[2];
            line[0] = '1';
            line[1] = '\0';
            TTF_SizeText(_font, line, &lX, &lY);

            char *l = itoa(y + 1, 10);

            graphics::Color c(_lang.colorScheme.defaultFG.r + 40, _lang.colorScheme.defaultFG.g + 40, _lang.colorScheme.defaultFG.b + 40, 255);
            graphics::Color bgC(_lang.colorScheme.defaultBG.r - 20, _lang.colorScheme.defaultBG.g - 20, _lang.colorScheme.defaultBG.b - 20, 255);
            for(unsigned int i = 0; i < lnDigits; ++i)
            {
                graphics::FontRenderer::setFont(_lineNumFont);
                int w, __h;
                TTF_SizeText(_lineNumFont, std::string(lineNumString).substr(0, i).c_str(), &w, &__h);
                graphics::FontRenderer::renderLetter(l[i], w + 2, (y - _scrollY) * (lY - 1), c, bgC);
            }


            std::vector<text::EditorChar> curLine = getEditorCharVector(lines[y], _lang.colorScheme);
            for(unsigned int x = _scrollX; x < curLine.size(); ++x)
            {
                char line[x - _scrollX + 1];
                for(unsigned int i = 0; i < x; ++i)
                {
                    line[i] = (*(_lines[y]))[i + _scrollX];
                }
                line[x - _scrollX] = '\0';
                TTF_SizeText(_font, line, &lX, &lY);
                // std::cout << lY << std::endl;

                /*
                if(x == _scrollX)
                {
                    char *l = itoa(y + 1, 10);

                    graphics::Color c(_lang.colorScheme.defaultFG.r + 40, _lang.colorScheme.defaultFG.g + 40, _lang.colorScheme.defaultFG.b + 40, 255);
                    graphics::Color bgC(_lang.colorScheme.defaultBG.r - 20, _lang.colorScheme.defaultBG.g - 20, _lang.colorScheme.defaultBG.b - 20, 255);
                    for(unsigned int i = 0; i < lnDigits; ++i)
                    {
                        graphics::FontRenderer::setFont(_lineNumFont);
                        int w, __h;
                        TTF_SizeText(_lineNumFont, std::string(lineNumString).substr(0, i).c_str(), &w, &__h);
                        graphics::FontRenderer::renderLetter(l[i], w + 2, (y - _scrollY) * (lY - 1), c, bgC);
                        graphics::FontRenderer::setFont(_font);
                    }
                }
                //*/

                bool inSel;
                for(unsigned int i = 0; i < _selections.size(); ++i)
                {
                    inSel = inSelection(x, y, _selections[i]);

                    if(inSel)   break;
                }

                Color selBg(_lang.colorScheme.defaultBG.r + 20, _lang.colorScheme.defaultBG.g + 20, _lang.colorScheme.defaultBG.b + 20, 255);

                // std::cout << (int) (lX + x * _spacing + 2 + lnRectDst.w) << " vs. " << (int) (lX + x * _spacing + 2 + lnRectDst.w - scrollW) << std::endl;
                graphics::FontRenderer::renderLetter(curLine[x].content, (int) (lX + (x - _scrollX) * _spacing + 2 + lnRectDst.w), (int) ((y - _scrollY) * (lY - 1)), curLine[x].fgColor, (inSel ? selBg : curLine[x].bgColor), _spacing);
            }
        }

        // Draw the caret
        if(_caretVisible)
        {
            for(unsigned int i = 0; i < _cursors.size(); ++i)
            {
                int &_cursorX = _cursors[i].x, &_cursorY = _cursors[i].y;

                int lX, lY;

                char line[_cursorX - _scrollX + 1];
                for(unsigned int i = 0; i < _cursorX; ++i)
                {
                    if(i + _scrollX >= _lines[_cursorY]->size())
                        break;

                    line[i] = (*(_lines[_cursorY]))[i + _scrollX];
                }
                line[_cursorX - _scrollX] = '\0';

                TTF_SizeText(_font, line, &lX, &lY);

                if(_debug)
                {
                    // Draw some reference lines
                    graphics::Color col(255, 0, 0, 255);
                    graphics::line(_target, 0, (_cursorY - _scrollY) * (_fontHeight - 1), lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 1), col);

                    col.r = 0;
                    col.b = 255;
                    graphics::line(_target, 0, (_cursorY - _scrollY) * (_fontHeight - 1) + (_fontHeight - 2), lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 1) + (_fontHeight - 1), col);

                    col.b = 0;
                    col.g = 255;
                    graphics::line(_target, lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, 0, lX + _cursorX * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 1), col);
                }

                 // std::cout << "Drawing caret at y " << (_cursorY - _scrollY) * (_fontHeight - 2) << " vs " << (_cursorY - _scrollY) * (_fontHeight - 2) + (_fontHeight - 2) << std::endl;
                graphics::line(_target, lX + (_cursorX - _scrollX) * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 1), lX + (_cursorX - _scrollX) * _spacing + 2 + lnRectDst.w - _spacing / 2, (_cursorY - _scrollY) * (_fontHeight - 1) + (_fontHeight - 2), _lang.colorScheme.caretFG);
            }
        }

        SDL_Rect pos;
        pos.x = _posX;
        pos.y = _posY;
        SDL_BlitSurface(_target, 0, _screen, &pos);
        GraphicsManager::flip();
    }
}
