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
 - SDL (http://libsdl.org)

How to compile on any platform(assuming you have Code::Blocks installed
and cofigured, and that you have installed SDL correctly)

- Open the file 'TextEditor.cbp' in Code::Blocks(File -> Open -> TextEditor.cbp)
- Check in the "Compiler & Debugger" settings, that it has these compiler options: "-std=c++11", and these 
	linker options: "-lSDL -lSDLmain".
- Press "Build".
- Run the executable produced by Code::Blocks. You will have to copy colorschemes/ into the folder with the executable,
	or add colorschemes/ parent folder to your system path.
