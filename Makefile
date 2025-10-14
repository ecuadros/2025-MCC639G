CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -pthread # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
SRCS = main.cpp \
	test.cpp  util.cpp \
	clasestemplate.cpp \
	pointers.cpp \
	ContainersDemo.cpp \
	vector.cpp \
	linkedlist.cpp \
	
#doublelinkedlist.cpp

	   	
#>>>>>>> 817223c (implementando copy constructor)
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
