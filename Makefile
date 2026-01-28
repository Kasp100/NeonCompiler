CXX = g++
CXXFLAGS = -std=c++20 -Wall -fmax-errors=1

# List of package directories
DEFAULT_PACKAGE_DIRS := . logging file_reading reading neon_compiler neon_compiler/lexer neon_compiler/parser neon_compiler/analysis/impl neon_compiler/ast/impl

IS_TEST := $(if $(MAKECMDGOALS),true,false)

# Change if tests are being run
ifeq ($(IS_TEST),true)
PACKAGE_DIRS := $(MAKECMDGOALS)
else
PACKAGE_DIRS := $(DEFAULT_PACKAGE_DIRS)
endif

# Collect all source files from the _package.txt files
SOURCES := $(foreach dir, $(PACKAGE_DIRS), $(addprefix $(dir)/, $(shell cat $(dir)/_package.txt)))

# Append ".cpp" to each source file name
SOURCES_CPP := $(addsuffix .cpp, $(SOURCES))

# Define the output executable name
ifeq ($(IS_TEST),true)
TARGET = test_runner
else
TARGET = build
endif

# Default target
all: $(TARGET)

$(PACKAGE_DIRS): $(TARGET)

# Rule to create the executable directly from source files
$(TARGET): $(SOURCES_CPP)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@if [ "$(IS_TEST)" = "true" ]; then \
		echo "Building and running test: $@"; \
		echo "Using packages: $(PACKAGE_DIRS)"; \
		./$(TARGET); \
		rm -f $(TARGET); \
	fi

# Clean rule to remove the target
clean:
	rm -f $(TARGET)
