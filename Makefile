# Compiler and compiler flags
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2 -g

# Automatically find all C++ source files in the current directory
SRCS := $(wildcard *.cpp)

# Use the parent directory's name for the final executable
EXEC_NAME := $(notdir $(CURDIR))
BUILD_DIR := target

# Generate object file paths in the build directory
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
EXEC := $(BUILD_DIR)/$(EXEC_NAME)

# Phony targets don't represent files
.PHONY: all clean run rebuild

# The default target, 'all', depends on the executable
all: $(EXEC)

# Rule to link the executable
# Depends on all object files and creates the build directory if it doesn't exist
$(EXEC): $(OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile C++ source files into object files
# Creates the build directory if it doesn't exist
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to run the executable
run: $(EXEC)
	./$(EXEC)

# Rule to clean the project by removing the build directory
clean:
	rm -rf $(BUILD_DIR)

# Rule to force a rebuild (clean and then build)
rebuild: clean all
