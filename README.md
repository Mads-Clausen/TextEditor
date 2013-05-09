TextEditor
==========

A text editor written in C++.

This is a screenshot of its current state:
![Screenshot](http://anickyan.koding.com/img/editor3.png)

Notice the multiple cursors. Screenshot with debug mode enabled:
![Screenshot](http://anickyan.koding.com/img/editor3_dbg.png)

Compilation
===========

In order to compile this application, you will need a valid C++ compiler,
capable of compiling code which relies on C++11 standards.

External libraries used:
* SDL (http://libsdl.org)
* SDL_ttf

For information about how to install and set up SDL 
and SDL_ttf, visit http://lazyfoo.net/SDL_tutorials/lesson01/index.php

For information about compiling for specific platforms, see COMPILING.md

If you are on Linux, you will also need to install xclip:
```
sudo apt-get install xclip
```
Alternatively use pacman or yum instead of apt-get

Features
========

* Syntax highlighting for C++
* Pasting
* Cursor positioning with mouse
* Debug mode - press F1 to enable
* Multiple editors. Press PAGE DOWN to select #1, PAGE UP to select #2
* Selections. Click one place, and shift-click somewhere else to select.

Known bugs
==========

None, currently.
For newest bugs, see the "Issues" tab.
