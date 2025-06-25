CXX = g++
CXXFLAGS = -std=c++17 -Wall

# List of package directories
PACKAGE_DIRS := . logging file_reading reading neon_compiler neon_compiler/tokenisation

# Collect all source files from the _package.txt files
SOURCES := $(foreach dir, $(PACKAGE_DIRS), $(addprefix $(dir)/, $(shell cat $(dir)/_package.txt)))

# Append ".cpp" to each source file name
SOURCES_CPP := $(addsuffix .cpp, $(SOURCES))

# Define the output executable name
TARGET = build

# Default target
all: $(TARGET)

# Rule to create the executable directly from source files
$(TARGET): $(SOURCES_CPP)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule to remove the target
clean:
	rm -f $(TARGET)
