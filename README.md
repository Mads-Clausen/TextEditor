TextEditor
==========

A text editor written in C++.

This is a screenshot of its current state:
![Screenshot](http://i.imgur.com/pwEpTrp.png)

Compilation
===========

In order to compile this application, you will need a valid C++ compiler,
capable of compiling code which relies on C++11 standards.

External libraries used:
* SDL (http://libsdl.org)
* SDL_ttf

For information about how to install and set up SDL 
and SDL_ttf, visit http://lazyfoo.net/SDL_tutorials/lesson01/index.php

How to compile on any platform(assuming you have Code::Blocks installed
and cofigured, and that you have installed SDL correctly)

* Open the file 'TextEditor.cbp' in Code::Blocks(File -> Open -> TextEditor.cbp)
* Check in the "Compiler & Debugger" settings, that it has these compiler options: "-std=c++11", and these 
	linker options: "-lSDL -lSDLmain -lSDL_ttf". You will also need to make sure that it has the
	"TextEditor/include/" folder listed under "Search Directories".
* Press "Build".
* Run the executable produced by Code::Blocks. You will have to copy "colorschemes/", "languages/"
	and "fonts/" into the folder with the executable, or to your system path.
