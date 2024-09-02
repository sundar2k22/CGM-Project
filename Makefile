build:
	g++ -g main.cpp glad/src/glad.c -I glad/include -I glfw/include -framework Cocoa -framework IOKit glfw/lib-arm64/libglfw3.a -Wall -Wextra -std=c++20 -o app