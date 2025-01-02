CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = stock_market
SRCS = main.cpp Order.cpp Orderbook.cpp
OBJS = $(SRCS:.cpp=.o)
HEADERS = Order.h Orderbook.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) output*.txt main
