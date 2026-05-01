CXX := g++
CXXFLAGS := -Wall -std=c++17
SRC := main.cpp Card.cpp Deck.cpp Enemy.cpp Player.cpp menu.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := game

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
