all:
	g++ -c Main.cpp -std=c++11
	g++ -o Main Main.o -l SDL2 -l SDL2_image -l GL -l GLEW
	rm Main.o
