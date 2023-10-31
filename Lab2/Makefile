# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -fopenmp

# Source and object files
SRCS = ECE_ElectricField.cpp ECE_PointCharge.cpp main.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = my_program
# Target zip
ZIPNAME = Lab2_code.zip

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

zip:
	zip $(ZIPNAME) *

clean:
	rm -f $(OBJS) $(TARGET)
	
.PHONY: all clean
