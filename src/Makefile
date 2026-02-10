CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Icomponents -Iinclude

TARGET = app
SRCS = main.cpp components/client/client.cpp components/server/server.cpp components/logging/logging.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)
	rm -f $(OBJS)
	rm -f main.o
	mv app ..

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)