all:
	g++ -std=c++11 -I./include -lSDL -lSDLmain -lSDL_ttf -lGL src/*.cpp src/text/*.cpp src/graphics/*.cpp -o TextEditor
