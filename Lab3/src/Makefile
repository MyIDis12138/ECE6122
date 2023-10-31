# Compiler
CXX = /usr/bin/g++ 

# Flags
CXXFLAGS = -I../

LIBS = -lGL -lGLEW -lglfw

# Sources
COMMON_SOURCES = ../common/controls.cpp ../common/objloader.cpp ../common/shader.cpp ../common/texture.cpp ../common/vboindexer.cpp

all: main

main: main.cpp $(COMMON_SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f main

.PHONY: all clean
