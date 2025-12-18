//
// Created by creeper on 7/20/24.
//
#include <meshark/mesh-simplifier.h>
// #include <format>
#include <fmt/core.h>

namespace meshark {

Vertex MeshSimplifier::collapseEdge(Edge e) {
  // TODO: Implement this function
  assert(mesh.isCollapsable(e));

  // 6 条半边
  HalfEdge h = e->halfEdge(); // 目标半边1
  HalfEdge h_twin = h->twin; // 目标半边2
  HalfEdge h_left_next = h->next; // 左面中h的下一条半边
  HalfEdge h_left_prev = h_left_next->next; // 左面中h的上一条半边
  HalfEdge h_right_next = h_twin->next; // 右面中h的下一条半边
  HalfEdge h_right_prev = h_right_next->next; // 右面中h的上一条半边

  // 三条边
  // Edge e; // 传入的中间边
  Edge e_delete_left = h_left_prev->edge; // 左面中被删除的边
  Edge e_delete_right = h_right_next->edge; // 右面中被删除的边

  Edge e_keep_left = h_left_next->edge;
  Edge e_keep_right = h_right_prev->edge;

  // 一个点
  Vertex v_keep = h->tip; // 保留点
  Vertex v_remove = h->tail; // 被删除的点
  Vertex v_left = h_left_next->tip; // 左面顶点
  Vertex v_right = h_right_prev->tail; // 右面顶点

  // 两个面
  Face f_left = h->face; // 消除的左面
  Face f_right = h_twin->face; // 消除的右面


  // 1. 修改 v_remove 所有出边的 tail 为 v_keep，以及它们对边的 tip 为 v_keep
  for (HalfEdge h : v_remove->outgoingHalfEdges())
  {
    h->tail = v_keep;
    h->twin->tip = v_keep;
  }

  // 2. 保证 v_keep v_left v_right 指向的半边不是将被删除的半边
  v_keep->halfEdge() = h_right_prev->twin; // 必须是从 v_keep 发出的半边！！
  v_left->halfEdge() = h_left_next->twin;
  v_right->halfEdge() = h_right_next->twin;

  // 3. 缝合左右面的另外两条边
  // 修改半边的 twin
  h_left_next->twin->twin = h_left_prev->twin;
  h_left_prev->twin->twin = h_left_next->twin;
  h_right_next->twin->twin = h_right_prev->twin;
  h_right_prev->twin->twin = h_right_next->twin;
  // 修改左右面半边 twin 的 edge 成员为保留边
  h_left_next->twin->edge = h_left_prev->twin->edge = e_keep_left;
  h_right_next->twin->edge = h_right_prev->twin->edge = e_keep_right;

  // 修改半边所属 Edge 的 halfEdge 引用，保证其不指向即将被删除的半边
  e_keep_left->halfEdge() = h_left_next->twin;
  e_keep_right->halfEdge() = h_right_prev->twin;


  // 4. 清理现场
  // 移除附加数据
  Q.removeVertexData(v_remove);

  // 移除拓扑元素
  mesh.removeFace(f_left);
  mesh.removeFace(f_right);
  
  mesh.removeHalfEdge(h);
  mesh.removeHalfEdge(h_twin);
  mesh.removeHalfEdge(h_left_next);
  mesh.removeHalfEdge(h_left_prev);
  mesh.removeHalfEdge(h_right_next);
  mesh.removeHalfEdge(h_right_prev);

  // 边必须一条一条删，严格保证 Data 和 Mesh 同步
  // MeshSimplifier 使用 edge_collapse_cost 维护 EdgeData 数组，使用的是mesh索引，改变 cost 数组的大小并移动数据
  // HalfEdgeMesh 使用 removeEdge 维护 Edge 数组，会改变 mesh 的数组大小并更新索引
  // 连续 edge_collapse_cost.removeEdgeData 可能导致后面removeEdgeData操作用错误的索引访问数据
  
  // 1. 删除中间边 e
  eraseEdgeMapping(e);                   // 从 map 移除
  edge_collapse_cost.removeEdgeData(e);  // 从 vector 移除 (数据交换)
  mesh.removeEdge(e);                    // 从 mesh 移除 (元素交换，更新索引)

  // 2. 删除左侧边 e_delete_left
  eraseEdgeMapping(e_delete_left);
  edge_collapse_cost.removeEdgeData(e_delete_left);
  mesh.removeEdge(e_delete_left);

  // 3. 删除右侧边 e_delete_right
  eraseEdgeMapping(e_delete_right);
  edge_collapse_cost.removeEdgeData(e_delete_right);
  mesh.removeEdge(e_delete_right);


  mesh.removeVertex(v_remove);
  return v_keep;
}

MeshSimplifier::MinCostEdgeCollapsingResult MeshSimplifier::collapseMinCostEdge() {
  auto min_cost_edge = cost_edge_map.begin()->second;
  // TODO: finish this function

  if (!mesh.isCollapsable(min_cost_edge)) {
    return {min_cost_edge, false};
  }

  glm::vec3 pos_opt = computeOptimalCollapsePosition(min_cost_edge);
  Vertex v_survive = collapseEdge(min_cost_edge);
  updateVertexPos(v_survive, pos_opt);

  return {min_cost_edge, true};
}

Real MeshSimplifier::computeEdgeCost(Edge e) const {
  // TODO: Implement this function
  glm::vec3 v_opt = computeOptimalCollapsePosition(e);
  glm::vec4 v_hom(v_opt, 1);
  Vertex v1 = e->firstVertex();
  Vertex v2 = e->secondVertex();

  glm::mat4 Q1 = Q(v1); 
  glm::mat4 Q2 = Q(v2);
  glm::mat4 Q_sum = Q1 + Q2;

  
  return glm::dot(v_hom, Q_sum * v_hom);
  // return 0.0;
}

void MeshSimplifier::runSimplify(Real alpha) {
  for (auto v : mesh.vertices())
    Q(v) = computeQuadricMatrix(v);
  for (auto e : mesh.edges()) {
    edge_collapse_cost(e) = computeEdgeCost(e);
    cost_edge_map.insert({edge_collapse_cost(e), e});
  }
  int round = 0;
  while (mesh.numEdges() > alpha * num_original_edges) {
    auto result = collapseMinCostEdge();
    round++;
    std::cout << fmt::format("Round {}: ", round);
    if (!result.is_collapsable) {
      auto e = result.failed_edge;
      updateEdgeCost(e, std::numeric_limits<Real>::infinity());
      std::cout << "Min-cost edge is not collapsable, skip\n";
      continue;
    }
    std::cout << fmt::format("{} edges left\n", mesh.numEdges());
  }
}

glm::vec3 MeshSimplifier::computeOptimalCollapsePosition(Edge e) const {
  // TODO: implement this function
  Vertex v1 = e->firstVertex();
  Vertex v2 = e->secondVertex();

  glm::mat4 Q1 = Q(v1); 
  glm::mat4 Q2 = Q(v2);
  glm::mat4 Q_sum = Q1 + Q2;

  // 修改Q_sum最后一行 (Row 3)
  glm::mat4 A = Q_sum;
  A[0][3] = 0.0f; 
  A[1][3] = 0.0f;
  A[2][3] = 0.0f; 
  A[3][3] = 1.0f;

  // 目标向量 b = [0, 0, 0, 1]
  glm::vec4 b(0.0f, 0.0f, 0.0f, 1.0f);

  // 尝试求解方程组
  // 检查行列式是否接近 0，防止除以零或数值不稳定
  if (std::abs(glm::determinant(A)) > 1e-7) {
    // 矩阵可逆，直接求解: x = A^-1 * b
    glm::mat4 A_inv = glm::inverse(A);
    glm::vec4 v_opt = A_inv * b;
    return glm::vec3(v_opt); // 隐式转换为 vec3 (丢弃 w)
  }

  // 如果矩阵不可逆，在两个端点和中点之间选择代价最小的位置
  glm::vec3 p1 = mesh.pos(v1);
  glm::vec3 p2 = mesh.pos(v2);
  glm::vec3 p_mid = (p1 + p2) * 0.5f;

  // 定义一个临时的 lambda 来计算代价: v^T * Q * v
  auto calculate_cost = [&](const glm::vec3& p) -> double {
    glm::vec4 v(p, 1.0f);
    // glm::dot(v, Q * v) 等价于 v^T * Q * v
    return glm::dot(v, Q_sum * v);
  };

  double cost1 = calculate_cost(p1);
  double cost2 = calculate_cost(p2);
  double cost_mid = calculate_cost(p_mid);

  // 返回代价最小的位置
  if (cost_mid < cost1 && cost_mid < cost2) return p_mid;
  if (cost1 < cost2) return p1;
  return p2;
}

void MeshSimplifier::updateVertexPos(Vertex v, const glm::vec3 &pos) {
  // TODO: implement this function
  // 设置点坐标
  mesh.setVertexPos(v, pos);

  // 记录需要更新的点
  std::vector<Vertex> vertices_to_update;
  vertices_to_update.push_back(v);
  for (HalfEdge h : v->outgoingHalfEdges()) {
      vertices_to_update.push_back(h->tip);
  }

  // 更新这些顶点的Q矩阵，并记录需要更新的边
  // 定义一个 Lambda 比较器用于 set
  auto edgeComparator = [](Edge a, Edge b) {
      return a.get() < b.get(); // 比较原始指针地址
  };
  // 使用自定义比较器的 set 来收集受影响的边
  std::set<Edge, decltype(edgeComparator)> edges_to_update(edgeComparator);

  for (Vertex u : vertices_to_update) {
      Q(u) = computeQuadricMatrix(u);
      // 收集该顶点连接的所有边
      for (HalfEdge h : u->outgoingHalfEdges()) {
          edges_to_update.insert(h->edge);
      }
  }

  // 更新这些边的代价（移出和重新插入优先队列由updateEdgeCost处理）
  for (Edge e : edges_to_update) {
      Real new_cost = computeEdgeCost(e);
      updateEdgeCost(e, new_cost); 
  }
}

glm::mat4 MeshSimplifier::computeQuadricMatrix(Vertex v) const {
  // TODO: implement this function
  // ax + by + cz + d = 0 
  // n = [a, b, c]^T
  // p = [a, b, c, d]^T
  // K_p = p * p^T
  // Q_v = \sum_{f \in faces(v)} K_{p,f}

  glm::mat4 Q(0.0f);
  for (HalfEdge h : v->outgoingHalfEdges())
  {
    Face f = h->face;
    
    if (!f) continue; // 防止边界情况
    glm::vec3 n = mesh.normal(f); // 平面法线
    // 利用 v 坐标求出 d
    glm::vec3 v_pos = mesh.pos(v);
    float d = -glm::dot(n, v_pos);

    glm::vec4 p(n.x, n.y, n.z, d);
    glm::mat4 Kp = glm::outerProduct(p, p);
    Q += Kp;
  }

  return Q;
}

void MeshSimplifier::eraseEdgeMapping(Edge e) {
  Real cost = edge_collapse_cost(e);
  auto range = cost_edge_map.equal_range(cost);
  // assert(range.first != cost_edge_map.end());
  if (range.first == cost_edge_map.end()) return;
  for (auto it = range.first; it != range.second; ++it) {
    if (it->second == e) {
      cost_edge_map.erase(it);
      break;
    }
  }
}
}