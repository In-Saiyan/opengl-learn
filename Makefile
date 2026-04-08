main: ./main.cpp ./include ./src
	g++ main.cpp -std=c++23 src/glad.c -I./include -o main -lglfw -lGL -lX11 -lpthread -ldl
