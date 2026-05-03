# Object files (.o) are not portable across OS or compilers.
# If you see MinGW/Windows symbols while linking on Linux, run: make clean && make
CXX := g++
CXXFLAGS := -Wall -std=c++17
SRC := main.cpp Card.cpp Deck.cpp Enemy.cpp Player.cpp menu.cpp Level.cpp SaveLoad.cpp BattleUI.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := game

# Detect OS for executable extension
ifeq ($(OS),Windows_NT)
    EXECUTABLE := $(TARGET).exe
else
    EXECUTABLE := $(TARGET)
endif

.PHONY: all clean run debug clean-obj rebuild help

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the game
run: $(TARGET)
	./$(EXECUTABLE)

# Compile with debug flags
debug: CXXFLAGS += -g -O0
debug: clean all

# Clean all build artifacts
clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).exe

# Clean only object files
clean-obj:
	rm -f $(OBJ)

# Full rebuild (use after copying project from another OS or if link fails oddly)
rebuild: clean all

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build the game (default)"
	@echo "  run       - Build and run the game"
	@echo "  debug     - Build with debug flags"
	@echo "  clean     - Remove all build files"
	@echo "  clean-obj - Remove only object files"
	@echo "  rebuild   - clean then build (fixes stale .o from another OS)"
	@echo "  help      - Show this help message"

