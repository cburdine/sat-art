
FLAGS=-O2 -L/usr/X11R6/lib -lm -lpthread -lX11

build:
	g++ -std=c++11 -o ./bin/sat_art ./src/*.cpp $(FLAGS)

