TextEditor
==========

A text editor written in C++.


Compilation
===========

In order to compile this application, you will need a valid C++ compiler,
capable of compiling code which relies on C++11 standards.

External libraries used:
 - SDL (http://libsdl.org)

How to compile on any platform(assuming you have Code::Blocks installed
and cofigured, and that you have install SDL correctly)

- Open the TextEditor.cbp in Code::Blocks
- Check in the "Compiler & Debugger" settings, that it has these compiler options: "-std=c++11", and these 
<<<<<<< HEAD
	linker options: "-lSDL -lSDLmain".
=======
 	linker options: "-lSDL -lSDLmain".
>>>>>>> f3e9e295973e853384779a71b671d4022e4c24e0
- Press "Build"
- Run the executable produced by Code::Blocks. You will have to copy colorschemes/ into the folder with the executable,
	or add colorschemes/ parent folder to your system path.
