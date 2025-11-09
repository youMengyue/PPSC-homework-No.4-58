# Makefile

# 编译器
CXX = g++

# 编译选项
# -std=c++17: 使用 C++17 标准
# -O3:        开启最高级别的优化，这对性能测试至关重要！
# -Wall:      显示所有警告
# -msse2:     显式启用 SSE2 指令集，_mm_div_pd 属于该指令集
CXXFLAGS = -std=c++17 -O3 -Wall -msse2

# 目标可执行文件名
TARGET = homework_58

# 源文件
SRCS = main.cpp

# 默认目标
all: $(TARGET)

# 链接规则
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# 运行目标
run: all
	./$(TARGET)

# 清理目标
clean:
	rm -f $(TARGET)