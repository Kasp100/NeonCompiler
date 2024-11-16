CXX = g++
CXXFLAGS = -std=c++17 -Wall

# List of package directories
PACKAGE_DIRS := .

include $(foreach dir, $(PACKAGE_DIRS), $(dir)/_package.mk)

# Append ".cpp" to each source file name
SOURCES_CPP := $(addsuffix .cpp, $(SOURCES))

# Generate the list of object files (.o) for each .cpp file
OBJ_FILES := $(addsuffix .o, $(basename $(SOURCES_CPP)))

# Define the output executable name
TARGET = build

# Default target
all: $(TARGET) clean_obj

# Rule to create the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile each .cpp file to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove object files and the target
clean:
	rm -f $(OBJ_FILES) $(TARGET)

clean_obj:
	rm -f $(OBJ_FILES)