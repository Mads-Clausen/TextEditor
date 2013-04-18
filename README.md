TextEditor
==========

A text editor written in C++.

This is a screenshot of its current state:
![Screenshot](http://anickyan.koding.com/img/editor2.png)

Compilation
===========

In order to compile this application, you will need a valid C++ compiler,
capable of compiling code which relies on C++11 standards.

External libraries used:
* SDL (http://libsdl.org)
* SDL_ttf

For information about how to install and set up SDL 
and SDL_ttf, visit http://lazyfoo.net/SDL_tutorials/lesson01/index.php

Compilation instructions. These assume you have Code::Blocks installed
and cofigured, and that you have installed SDL correctly:

* Open the file 'TextEditor.cbp' in Code::Blocks(File -> Open -> TextEditor.cbp).
* Go to "Settings -> Compiler & Debugger"
	* Insert "-std=c++11" into "Compiler Settings -> Other options".
	* Insert "-lSDL -lSDLmain -lSDL_ttf" into "Linker Settings -> Other linker options".
	* Make sure that "<folder/you/downloaded/to>/include/" is listed in "Search Directories".
* Press "Build".
* Press "Run".

If you are on Linux, you will also need to install xclip:
```
sudo apt-get install xclip
```
Alternatively use pacman or yum instead of apt-get


Known bugs
==========

Sometimes, the caret will be drawn at an incorrect position.
The cursor is at the right position, though.
