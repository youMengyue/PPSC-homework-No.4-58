# Makefile

# Compiler
CXX = g++

# --- Standard flags with optimization and auto-vectorization ---
CXXFLAGS_VEC = -std=c++17 -O3 -Wall -msse2

# --- Flags with optimization BUT with DISABLED auto-vectorization ---
CXXFLAGS_NO_VEC = -std=c++17 -O3 -Wall -msse2 -fno-tree-vectorize

# Output executable name
TARGET = homework_58

# Source files
SRCS = main.cpp

# Default target (with auto-vectorization)
all:
	$(CXX) $(CXXFLAGS_VEC) -o $(TARGET) $(SRCS)

# Run version with auto-vectorization
run: all
	./$(TARGET)

# --- New targets for experiment ---

# Build version without auto-vectorization
build_no_vec:
	$(CXX) $(CXXFLAGS_NO_VEC) -o $(TARGET) $(SRCS)

# Run version without auto-vectorization to see REAL difference
run_no_vec: build_no_vec
	@echo ""
	@echo "--- Running version with DISABLED auto-vectorization ---"
	./$(TARGET)

# Clean target
clean:
	rm -f $(TARGET)
