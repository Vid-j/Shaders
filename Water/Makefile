# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I. -Wall

# Libraries
LIBS = -lGL -lGLEW -lglfw

# Source files (no main.cpp now!)
SRCS = A6-Water.cpp camera.cpp shader_utils.cpp
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = water

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
