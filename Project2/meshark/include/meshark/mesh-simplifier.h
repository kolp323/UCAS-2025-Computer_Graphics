//
// Created by creeper on 7/20/24.
//

#ifndef MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_MESH_SIMPLIFIER_H_
#define MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_MESH_SIMPLIFIER_H_

#include <meshark/geometry-mesh.h>
#include <meshark/element-data.h>
#include <set>
#include <map>

namespace meshark {
using Real = double;

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

  void updateEdgeCost(Edge e, Real updated_cost) {
    if (edge_collapse_cost(e) == updated_cost)
      return;
    eraseEdgeMapping(e);
    edge_collapse_cost(e) = updated_cost;
    cost_edge_map.insert({updated_cost, e});
  }
};
}
#endif //MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_MESH_SIMPLIFIER_H_
