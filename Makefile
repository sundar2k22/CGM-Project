# # Compiler and flags
# CC = g++
# CFLAGS = -Wall -Wextra -std=c++11

# # Include paths for libraries
# INCLUDES = -I/usr/include -I/usr/local/include

# # Libraries to link with (GLFW, GLEW, OpenGL)
# LIBS = -lglfw -lGLEW -lGL -lm

# # Source files and object files
# SRCS = main.cpp
# OBJS = $(SRCS:.cpp=.o)

# # Name of the output executable
# MAIN = sphere

# # Rule to build the executable
# $(MAIN): $(OBJS)
# 	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

# # Rule to build object files from source files
# .cpp.o:
# 	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# # Clean rule to remove object files and the executable
# clean:
# 	$(RM) *.o *~ $(MAIN)
# Compiler and flags
# Compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11

# Include paths for libraries
INCLUDES = -I/opt/homebrew/opt/glew/include -I/opt/homebrew/opt/glfw/include

# Libraries to link with (GLFW, GLEW, OpenGL)
LIBS = -L/opt/homebrew/opt/glew/lib -L/opt/homebrew/opt/glfw/lib -lglfw -lGLEW -framework OpenGL -lm

# Source files and object files
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Name of the output executable
MAIN = sphere

# Rule to build the executable
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

# Rule to build object files from source files
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule to remove object files and the executable
clean:
	$(RM) *.o *~ $(MAIN)
