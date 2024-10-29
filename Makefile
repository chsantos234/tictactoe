# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -pthread

# Target executable
TARGET = tictactoe

# Source files
SRCS = tictactoe.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to build the object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build (removes executable and object files)
clean:
	rm -f $(OBJS) $(TARGET)
