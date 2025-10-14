CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wunused-value -g -pthread # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
SRCS = main.cpp \
	   test.cpp  util.cpp \
	   clasestemplate.cpp \
	   pointers.cpp \
	   linkedlist.cpp\
	   concurrentlinkedlist.cpp\
	   
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean