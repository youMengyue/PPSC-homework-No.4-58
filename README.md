This line is test
# SIMD Homework — Fan Zhanhong (ID 58)
# Task: Use _mm_div_pd (SSE2 double division)

CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -msse2
SRC = src/main.cpp
TARGET = simd_task

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
