# UCAS-2025-Computer_Graphics

本仓库整理 UCAS 2025 秋季《计算机图形学》课程资料与实验代码，内容以 OpenGL 图形绘制、网格简化和光线跟踪为主，包含三个课程项目以及一个基础 OpenGL 启动示例。

## 仓库结构

```text
.
├── Project1/      # OpenGL 基础图形绘制与交互窗口
├── Project2/      # 基于 QEM 的三角网格简化
├── Project3/      # 基于 KD-Tree 加速的光线跟踪渲染器
├── start/         # OpenGL 基础启动示例
├── .vscode/       # VS Code 配置
└── Readme.md      # 仓库总览
```

## 内容概览

### Project1：OpenGL 基础图形绘制

`Project1` 包含计算机图形学基础绘制实验，主要使用 OpenGL / GLUT 实现二维图形绘制、区域填充和交互控制。

目录内容包括：

- `teapot`：OpenGL 测试程序
- `basic_graphics`：基本图形绘制实验
- `color_filling`：平面多边形区域填充实验
- `interactive_window`：交互式窗口绘图实验
- `fig`：实验效果截图
- `makefile`：项目编译脚本

该项目涉及的主要内容：

- OpenGL 基本图元绘制
- 空心矩形、圆、圆角矩形、三角形等图形封装
- 实心图形与颜色填充
- 鼠标交互绘制
- GLUT 菜单、颜色选择和绘图模式切换
- 历史图形记录与窗口重绘

### Project2：Mesh Simplification

`Project2` 是三角网格简化项目，基于 Quadric Error Metric（QEM）算法对 OBJ 网格模型进行简化。

目录内容包括：

- `assets`：原始网格模型，包含 armadillo、bunny、sphere、spot、torus 等 OBJ 文件
- `meshark/apps`：网格简化程序入口
- `meshark/include`：半边结构、网格元素和简化器接口
- `meshark/src`：网格拓扑操作与 QEM 算法实现
- `external/glm`：GLM 数学库依赖
- `output`：不同简化率下的输出模型和效果图
- `CMakeLists.txt`：CMake 构建配置
- `README.md` / `README.pdf`：项目详细说明

该项目涉及的主要内容：

- 半边数据结构
- 顶点邻域遍历
- QEM 二次误差矩阵计算
- 边坍缩代价计算
- 最优坍缩点求解
- 网格拓扑缝合与元素删除
- 优先队列维护与级联更新
- 不同简化率下的网格效果对比

### Project3：Ray Tracing

`Project3` 是光线跟踪渲染项目，实现了一个带交互式相机控制的球体场景渲染器，并使用 KD-Tree 对求交过程进行加速。

目录内容包括：

- `include/element.h`：向量与球体类定义
- `include/kd_tree.h`：KD-Tree 与 AABB 相关函数
- `include/trace.h`：光线跟踪相关函数声明
- `include/stb_image_write.h`：PNG 图像输出依赖
- `src/main.cpp`：程序主逻辑与交互控制
- `src/trace.cpp`：光线跟踪、渲染与求交函数实现
- `output`：渲染结果图像
- `makefile`：项目构建脚本

该项目涉及的主要内容：

- 光线与球体求交
- 点光源下的高光、漫反射和阴影效果
- AABB 包围盒相交测试
- KD-Tree 空间划分与递归遍历
- 交互式相机位姿控制
- OpenGL 像素缓冲区显示
- PNG 渲染结果保存

### start：OpenGL 启动示例

`start` 目录包含基础 OpenGL 示例程序，用于课程实验的初始环境测试和简单窗口程序启动。

目录内容包括：

- `start.cpp`：基础 OpenGL 示例源码
- `start`：编译生成的可执行文件
- `makefile`：编译脚本

## 技术栈

仓库代码主要涉及：

- C++ / C：课程项目主体实现
- OpenGL / GLUT / FreeGLUT：图形窗口、图元绘制和交互控制
- CMake / Makefile：项目构建
- Shell：批量构建和运行脚本
- GLM：网格简化项目中的数学计算
- stb_image_write：光线跟踪项目中的 PNG 图像输出

## 目录索引

| 路径 | 内容 |
| --- | --- |
| `Project1` | OpenGL 基础绘制、填充和交互窗口实验 |
| `Project2` | 基于 QEM 的三角网格简化项目 |
| `Project3` | 基于 KD-Tree 加速的光线跟踪渲染器 |
| `start` | OpenGL 基础启动示例 |
| `Project1/fig` | Project1 实验效果图 |
| `Project2/assets` | Project2 输入网格模型 |
| `Project2/output` | Project2 网格简化输出结果 |
| `Project3/output` | Project3 光线跟踪渲染结果 |