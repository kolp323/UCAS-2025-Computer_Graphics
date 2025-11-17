# ----------------------------------------------------
# 1. 变量定义
# ----------------------------------------------------
CXX := g++
CXXFLAGS := -g -Wall
LIBS := -lglut -lGLU -lGL

# 项目文件夹列表
DIRS := basic_graphics color_filling interactive_window teapot

# 可执行文件列表: e.g., ./basic_graphics/main ./color_filling/main ...
# 这是一个变量替换引用。它将 DIRS 列表中的每个元素 (%) 替换为 ./%/main
# 格式：  列表变量:%=有关%的表达式
EXECS := $(DIRS:%=./%/main)

# ----------------------------------------------------
# 2. 编译规则
# ----------------------------------------------------

# 默认目标: 编译所有项目
.PHONY: all # 声明 all 为虚假目标 (phony target)。这意味着它不是一个实际的文件名，make 总是会执行其命令。
all: $(EXECS) # all 目标依赖于 EXECS 变量中的所有可执行文件。执行 make all 时，Make 会尝试编译所有这些文件
	@echo "All projects compiled successfully."

# 模式规则: 用于编译任何目录下的 main.cpp 文件
# 目标: ./*/main  (例如：./teapot/main)
# 依赖: ./*/main.cpp (例如：./teapot/main.cpp)
# $<：自动变量，代表第一个依赖文件 (./<文件夹名>/main.cpp)
# $@：自动变量，代表当前目标文件 (./<文件夹名>/main)
./%/main: ./%/main.cpp
	@echo "Compiling $< to $@"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# ----------------------------------------------------
# 3. 运行目标 (Compile & Run)
# ----------------------------------------------------

# 定义 PHONY 目标，确保 'make <folder>' 总是执行命令
.PHONY: $(DIRS) # 声明所有文件夹名 (basic_graphics, teapot 等) 为虚假目标

# 为每个目录创建一个 'make <folder>' 目标
# 例如: 'make basic_graphics' 将编译并运行 ./basic_graphics/main
$(DIRS):
# 	递归 Make。这一步强制 Make 去执行目标 ./<当前目标名>/main 的编译规则。
	$(MAKE) ./$@/main
	@echo "--- Running $@ ---"
	# 运行可执行文件
	./$@/main

# ----------------------------------------------------
# 4. 清理目标
# ----------------------------------------------------
.PHONY: clean
clean:
	@echo "Removing all project executables..."
# 	# 删除所有目录下的 main 可执行文件
	rm -f $(EXECS)