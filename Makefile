CXX = g++
CXXFLAGS = -std=c++17 -Wall

# List of packages
PACKAGES := .

# Generate the list of .cpp files from each _package.mk
SOURCES := $(foreach dir, $(PACKAGES), $(shell cat $(dir)/_package.mk | grep -oP 'SOURCES\s*=\s*\K.*'))

# Append ".cpp" to each source file name
SOURCES_CPP := $(addsuffix .cpp, $(SOURCES))

# Generate the list of object files (.o) for each .cpp file
OBJ_FILES := $(addsuffix .o, $(basename $(SOURCES_CPP)))

# Define the output executable name
TARGET = build

# Default target
all: $(TARGET)

# Rule to create the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile each .cpp file to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include the module Makefiles
include $(foreach dir, $(PACKAGES), $(dir)/_package.mk)

# Clean rule to remove object files and the target
clean:
	rm -f $(OBJ_FILES) $(TARGET)
