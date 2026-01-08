# 编译器及选项
CXX = g++
# -Iinclude 告诉编译器在 include 文件夹中寻找头文件
# -O3 开启高级优化
CXXFLAGS = -Wall -g -Iinclude -O2

LDLIBS = -lglut -lGLU -lGL 

# 目录定义
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# 自动获取所有源文件并生成对应的对象文件路径
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/trace.cpp
# 将 src/*.cpp 映射为 build/*.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# 最终生成的可执行文件名
TARGET = $(BUILD_DIR)/main

# 默认目标
all: $(TARGET)

# 链接阶段：将所有 .o 文件链接成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
	@echo "编译成功！可执行文件位于: $(TARGET)"

# 编译阶段：将每个 .cpp 文件编译为 .o 文件
# 使用 -c 选项表示只编译不链接
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 运行程序
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

clean:
	rm -rf $(BUILD_DIR)