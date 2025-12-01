CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

# object files
OBJS = main.o replica.o

# final program name
TARGET = program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp replica.h messages.h
	$(CXX) $(CXXFLAGS) -c main.cpp

replica.o: replica.cpp replica.h messages.h
	$(CXX) $(CXXFLAGS) -c replica.cpp

clean:
	rm -f $(OBJS) $(TARGET)
