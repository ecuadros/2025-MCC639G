CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCS = main.cpp test.cpp
BUILD_DIR = target
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
EXEC = $(BUILD_DIR)/main

all: $(EXEC)

$(EXEC): $(OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
