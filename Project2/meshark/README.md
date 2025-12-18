# UCAS CG Project-2: QEM Mesh Simplification

## 实验总览

本次实验的目的在于：

- 了解基本的半边表操作
- 学习实现QEM（Quadric Error Metric）网格简化算法

本次作业码量有显著提升，请同学们合理安排时间。另外，由于课程中已经讲过了QEM网格简化的数学原理，对于各种数学计算，我们在这里就不提了，请同学们参考PPT或者课堂笔记。

## 框架说明

### 半边表数据结构

本次实验没有采用任何外部库的半边表结构，而是自行实现了一个简单的半边表结构。这么做的原因是，许多常见的开源库的半边表封装程度较高，考虑了到许多针对本实验而言不必要的功能，非常冗余。

因此，我们自行实现了一个轻量的，简单易读（但愿如此）的半边表结构，让大家自己实现一些基本操作。

#### 1. 半边表元素的使用

基本的半边表使用如下4个类用来保存拓扑结构信息：

```cpp
using HalfEdge = HalfEdgeElement *;
using Edge = EdgeElement *;
using Vertex = VertexElement *;
using Face = FaceElement *;

struct HalfEdgeElement {
  Vertex tip;
  Vertex tail;
  HalfEdge next;
  HalfEdge twin;
  Face face;
  Edge edge;
};

struct EdgeElement {
  [[nodiscard]] HalfEdge halfEdge() const {
    return he;
  }
  HalfEdge &halfEdge() {
    return he;
  }
  [[nodiscard]] Vertex firstVertex() const {
    return he->tip;
  }
  [[nodiscard]] Vertex secondVertex() const {
    return he->tail;
  }
 protected:
  HalfEdge he;
};

struct VertexElement {
  [[nodiscard]] HalfEdge halfEdge() const {
    return he;
  }
  HalfEdge &halfEdge() {
    return he;
  }
 protected:
  HalfEdge he;
};

struct FaceElement {
  [[nodiscard]] HalfEdge halfEdge() const {
    return he;
  }
  HalfEdge &halfEdge() {
    return he;
  }
 protected:
  HalfEdge he;
};

```

对于半边`HalfEdgeElement`， 其成员含义如下：

- `tip`： 返回半边指向的顶点
- `tail`： 返回半边的起始顶点
- `next`： 返回按照逆时针顺序绕行当前半边所在面片的下一个半边
- `twin`： 返回当前半边的对偶半边（也就是与当前半边共享一个边，方向相反的半边）
- `face`： 返回当前半边所在的面片

对于边`EdgeElement`， 可以通过`halfEdge`方法获取该边的一个半边，通过`firstVertex`和`secondVertex`方法获取该边的两个顶点。注意，这里的边是无向的，因此`firstVertex`和`secondVertex`的顺序是任意的。

对于顶点`VertexElement`， 可以通过`halfEdge`方法获取从该顶点出发的任意一个半边。

对于面片`FaceElement`， 可以通过`halfEdge`方法获取属于该面片的任意一个半边。

`XXXElement`类实际上很少直接使用。`Vertex`, `Edge`, `HalfEdge`, `Face`的本质都是指针，不掌管所有权，并且可以很方便地拷贝，推荐使用。

#### 2. 为元素附加数据

我们设计了三种模板类，用于给网格的顶点，边和面片附加额外的数据。这三种模板类分别是`VertexData`, `EdgeData`, `FaceData`。

```cpp
template <typename T>
struct VertexData {
   VertexData(int size);
   const T& operator()(Vertex v) const;
   T& operator()(Vertex v);
   void removeVertexData(Vertex v);
};

template <typename T>
struct EdgeData {
   EdgeData(int size);
   const T& operator()(Edge e) const;
   T& operator()(Edge e);
   void removeEdgeData(Edge e);
};

template <typename T>
struct FaceData {
   FaceData(int size);
   const T& operator()(Face f) const;
   T& operator()(Face f);
   void removeFaceData(Face f);
};

```

在构造时传入预留的大小（也就是网格中该种元素的数量），然后可以通过`operator()`方法给每个元素附加一个`T`类型的数据。这些数据可以通过`operator()`方法读取和修改。当不再需要这些数据时，可以通过`removeXXXData`方法删除。

#### 3. 半边表的使用

在本次实验中，我们将要使用的半边表网格类是`GeometryMesh`类。除了保存拓扑结构之外，它还为所有顶点附加了`glm::vec3`类型的位置属性，为所有面附加了`glm::vec3`类型的法线属性。其可用方法如下：

- `GeometryMesh::halfEdges()`： 返回一个范围，包含所有的半边，可以用`range-for`语法遍历所有的半边。如下：

  ```cpp
     for (HalfEdge h : mesh.halfEdges()) {
       // do something with h...
     }
  ```
- `edges()`： 返回一个范围，包含所有的边，可以用`range-for`语法遍历所有的边。
- `vertices()`： 返回一个范围，包含所有的顶点，可以用`range-for`语法遍历所有的顶点。
- `faces()`： 返回一个范围，包含所有的面片，可以用`range-for`语法遍历所有的面片。
- `pos(Vertex v)`： 返回顶点`v`的位置。
- `normal(Face f)`： 返回面片`f`的法线。
- `setVertexPos(Vertex v, const glm::vec3 &pos)`： 设置顶点`v`的位置，并更新相邻面片的法线。
- `removeVertex(Vertex v)`： 删除顶点`v`。该操作不会删除任何其它相邻元素，因此往往不能单独使用。
- `removeEdge(Edge e)`： 删除边`e`。该操作不会删除任何其它元素，因此往往不能单独使用。
- `removeFace(Face f)`： 删除面片`f`。该操作不会删除任何其它元素，因此往往不能单独使用。
- `removeHalfEdge(HalfEdge h)`： 删除半边`h`。该操作不会删除任何其它元素，因此往往不能单独使用。
- `numVertices()`： 返回顶点的数量。
- `numEdges()`： 返回边的数量。
- `numFaces()`： 返回面片的数量。
- `isCollapsable(Edge e)`： 判断边`e`是否可以被坍缩。

这些方法对于本次实验中的网格简化操作应该足够了。

### 4. 网格简化器

这对应于`MeshSimplifier`类，它是一个简单的网格简化器，如下：

```cpp

struct MeshSimplifier {
  explicit MeshSimplifier(GeometryMesh &mesh)
      : mesh(mesh), Q(mesh.numVertices()), edge_collapse_cost(mesh.numEdges()), num_original_edges(mesh.numEdges()) {
  }
  void runSimplify(Real alpha);
  GeometryMesh &mesh;
 private:
  EdgeData<Real> edge_collapse_cost;
  std::multimap<Real, Edge> cost_edge_map;
  VertexData<glm::mat4> Q;
  [[nodiscard]] Vertex collapseEdge(Edge e);
  void eraseEdgeMapping(Edge e);
  int num_original_edges;
  struct MinCostEdgeCollapsingResult {
    Edge failed_edge;
    bool is_collapsable;
  };
  MinCostEdgeCollapsingResult collapseMinCostEdge();
  void updateVertexPos(Vertex v, const glm::vec3 &pos);
  [[nodiscard]] glm::mat4 computeQuadricMatrix(Vertex v) const;
  [[nodiscard]] Real computeEdgeCost(Edge e) const;
  [[nodiscard]] glm::vec3 computeOptimalCollapsePosition(Edge e) const;
  void updateEdgeCost(Edge e, Real updated_cost);
};

```

其方法说明如下：

- `runSimplify(Real alpha)`： 运行网格简化算法，`alpha`是一个控制简化程度的参数，取值范围为[0, 1]。`alpha`越大，简化程度越高。
- `collapseEdge(Edge e)`： 对边`e`进行坍缩操作，返回坍缩后的顶点。在本次实验中需要你实现。
- `eraseEdgeMapping(Edge e)`： 删除边`e`在`cost_edge_map`中的映射。
- `collapseMinCostEdge()`： 找到代价最小的边进行坍缩操作。返回一个结构体`MinCostEdgeCollapsingResult`，如果坍缩操作成功，`is_collapsable`为`true`，否则最小代价的边不可坍缩，`failed_edge`为该边。在本次实验中需要你实现。
- `updateVertexPos(Vertex v, const glm::vec3 &pos)`： 更新顶点`v`的位置为`pos`，并更新所有相关的法线，代价等信息。在本次实验中需要你实现。
- `computeQuadricMatrix(Vertex v)`： 计算顶点`v`的二次度量误差矩阵。在本次实验中需要你实现。
- `computeEdgeCost(Edge e)`： 计算边`e`的坍缩代价。在本次实验中需要你实现。
- `computeOptimalCollapsePosition(Edge e)`： 计算边`e`的最优坍缩位置。在本次实验中需要你实现。
- `updateEdgeCost(Edge e, Real updated_cost)`： 更新边`e`的坍缩代价为`updated_cost`。

## 课程实验：简化你的网格

### 1. 用迭代器遍历相邻半边

在网格简化实验中，一个基本的操作是：对于给定的一个顶点，遍历它的相邻半边。我们自然可以用`do while`循环来实现这个操作，但是每次都这么写未免也太麻烦了，因此，我们首先将这个操作封装为一个迭代器，这样我们就可以用`range-for`语法来遍历相邻半边了。

在你正确完成这个部分后，你应该可以这样遍历从顶点`v`出发的所有相邻半边：

```cpp

for (HalfEdge h : v->outgoingHalfEdges()) {
  // do something with h...
}

```

这样语义很清晰，代码也更简洁。

$\text{你的任务}$

在`include/mesh-elements.h`中，实现`VertexElement::OutgoingHalfEdgeRange::Iterator`的`operator++`方法，使得上述语法糖可以正常工作。

**提示**：

1. 上述语法糖会被编译器展开为类似下面的代码（可以拿cppinsights观察一下）：

   ```cpp

     for (Iterator it = v->outgoingHalfEdges().begin(); it != v->outgoingHalfEdges().end(); ++it) {
       HalfEdge h = *it;
       // do something with h...
     }

   ```

   你可以参看已经实现好的`begin`和`end`，你要做的就是正确实现`++it`的操作。
2. 你可以参看`FaceElement`类中的`BoundaryLoop`类的实现，这个类和你要实现的类有很多相似之处，观察它的`operator++`和`operator*`的实现。
3. 为了检查你的实现是否正确，你可以找一个顶点，遍历它的所有相邻半边，然后输出每个半边的`tip`和`tail`的信息，看看是否符合你的预期。
4. 还有其它检查的方法，例如，你可以使用range-for计算每个顶点的度数，将其求和。如果你的实现正确，那么顶点度数之和应该是边数的两倍。

### 2. 边坍缩操作

边坍缩操作是QEM网格简化算法的核心，也是本实验中最难的部分。

在`MeshSimplifier`类中，实现`collapseEdge`方法，该方法接受一个`Edge`类型的参数，表示要坍缩的边。并返回一个`Vertex`，表示边坍缩成的顶点。

需要注意的是，不是所有的边都可以进行坍缩操作，有些边是不允许被坍缩的，某些边在坍缩之后会破坏网格的流形性质。具体而言，如果被坍缩的边的两个顶点有超过2个公共的邻居，则该边不能被坍缩。因此，在本部分中，我们假设被坍缩的边都是合法的。

除此以外，在本部分中我们假设所有被坍缩的边都不处于边界上，这样带来另一个结论： 被坍缩的边一定有两个相邻的面。结合上一条，我们又可以知道，被坍缩的边的两个顶点一定有且仅有2个公共的邻居。

有了上面两个假设，边坍缩操作的实现就简化了许多。

$\text{你的任务}$

在`src/mesh-simplifier.cc`中，补全实现`MeshSimplifier::collapseEdge`方法。

**提示**：

1. 对于一个正确的边坍缩操作，每一次肯定需要减少1个顶点。假设被坍缩的边不是边界，以此为出发点，可以知道，必然会删除两个面，3个边，6个半边。
2. 在半边表上实现网格拓扑操作时，基本上可以分三步走：
   - 把所有涉及到的需要修改的元素（无论是什么类型）都通通先取出来保存为变量。
   - 修改这些变量的成员，使得它们符合新的拓扑关系。
   - 删除那些不再需要的元素，确保它们已经不在任何其它元素的指向中。
     按照这样的顺序操作，起码可以保证你不会因为先删除了某个元素导致其它指向它的指针/引用失效。
3. `MeshSimplier`本身维护了一些顶点和边的附加数据，在删除顶点和边之前，这些附加数据也需要被删除。
4. 为了检查你的实现是否正确，你可以在一个简单的网格上进行测试并且直接可视化。或者，你也可以在坍缩完成之后检查整个网格是否还符合流形性质。试着多做几轮坍缩，确保这一步你的实现是万无一失的。
5. 你可以在移除元素之前，先检查一下是否还有任何其它元素指向它，这样你能最早地发现问题。
6. 无论如何，这一步都是整个网格简化实验的核心，一旦出错，调试也比较麻烦。先打好草稿，再开始实现，并且多使用一些`assert`以及各种检查。考虑到出错后的调试代价，这一步无论多么小心也不为过。

### 3. 顶点/边代价的计算和更新

如果能够坍缩，最小代价的边会被坍缩至一个顶点，不妨记为`v`，这个顶点的位置由`computeOptimalCollapsePosition`方法计算得到。之后，你应该使用`mesh`的`setVertexPos`方法更新这个顶点的位置。`GeometryMesh`类会在设置顶点位置的同时自动帮你完成面片法线的更新。

由于与`v`相邻的面片法线都已经更新，处于这些面片上的顶点的二次度量误差矩阵也应该重新计算。这些顶点的Q矩阵更新，又将导致与这些顶点相邻的边的代价更新。按照这个顺序，更新所有相关信息。

$\text{你的任务}$

在`src/mesh-simplifier.cc`中，补全实现`MeshSimplifier::computeQuadricMatrix`，`MeshSimplifier::computeEdgeCost`，`MeshSimplifier::computeOptimalCollapsePosition`，`MeshSimplifier::computeQuadricMatrix`，`MeshSimplifier::updateVertexPos`方法。

**提示**：

1. 你不必太纠结性能，只要你没有傻傻地遍历所有的顶点/边/面片，你的实现应该都是可以接受的。在此基础上，怎么方便怎么来。
2. 对于向量$a$，可以使用`glm`的`outerProduct(a)`方法来方便地计算矩阵$aa^T$

### 4. 坍缩代价最小的边

找到坍缩代价最小的边，并且判断它是否能够被坍缩。如果不能被坍缩，返回`failed_edge`，否则执行坍缩操作，更新代价信息并返回`is_collapsable`为`true`。

$\text{你的任务}$

在`src/mesh-simplifier.cc`中，补全实现`MeshSimplifier::collapseMinCostEdge`方法。

**提示**：

我们在起始代码中已经把最小代价边给你找出来了，对它操作即可。

### 5. 如果你做对了...

在`assets`目录下有一些封闭的网格模型，你可以试着以他们为输入，运行你的网格简化器。把输出的网格模型扔进任意一个3D软件（或者你在第二次实验中编写的查看器）中，看看它的形状是否被保持。同时看看简化后的obj文件大小，看看简化率是否大体正确。

上交时，你需要把0.25, 0.5, 0.75三种简化率下得到的obj文件放在一个单独的output目录中，并附上一个简单的说明文档，在里面展示简化前后的截图以及必要的文字说明。

此外，我们要求你的代码在不同的编译器优化下都应该能够正常运行。你可以使用不同的编译选项，如果运行结果不一致，那么说明你的代码存在未定义行为，需要你仔细检查。

### 6. 选做： 边界的处理/自由发挥

你可以在这个实验的半边表/网格简化器的基础上，再自由发挥！比如：

- 我们在本次实验中没有处理边界，你可以尝试一下！
- 我们实现了QEM算法，你可以尝试实现一下其它的网格简化算法。
- 你可以尝试实现一下网格的细化算法，当然，这需要你实现新的拓扑操作，比如边分裂等。不过我相信有了实现边坍缩操作的经验，实现其它的拓扑操作也不会太难了。
- ...

无论你做了什么，都可以提交一份文档汇报你的努力。

### 7. 下期预告

在下一次实验中，我们将转入离线渲染部分，实现一个简单的路径追踪渲染器来渲染物理准确的真实感图像。
