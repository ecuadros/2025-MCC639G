# Makefile to wrap CMake build process

.PHONY: all configure build clean

# Default target: build the project
all: build

# Configure the project using CMake
# This creates the 'target' directory and generates the build system (e.g., Makefiles)
configure: 
	@echo "-- Configuring the project with CMake..."
	@cmake -S . -B target

# Build the project using the generated build system
# This depends on the configuration step being run first.
# We use the presence of CMakeCache.txt to check this.
build: target/CMakeCache.txt
	@echo "-- Building the project..."
	@cmake --build target

# The configuration is implicitly run if the cache file doesn't exist.
target/CMakeCache.txt: 
	@cmake -S . -B target

# Clean the build directory
clean:
	@echo "-- Cleaning the build directory..."
	@rm -rf target
