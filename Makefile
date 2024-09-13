# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Target executable names
TARGET1 = RatingDistance
TARGET2 = MovieRate

# Source files
SRC1 = RatingDistance.cpp
SRC2 = MovieRate.cpp

# Rule to build RatingDistance
$(TARGET1): $(SRC1)
	$(CXX) $(CXXFLAGS) -o $(TARGET1) $(SRC1)

# Rule to build MovieRate
$(TARGET2): $(SRC2)
	$(CXX) $(CXXFLAGS) -o $(TARGET2) $(SRC2)

# Build both targets with the 'all' rule
all: $(TARGET1) $(TARGET2)

# Clean up compiled files
clean:
	rm -f $(TARGET1) $(TARGET2)