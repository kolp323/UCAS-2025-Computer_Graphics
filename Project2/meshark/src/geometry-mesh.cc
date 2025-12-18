//
// Created by creeper on 7/22/24.
//
#include <meshark/geometry-mesh.h>
#include <meshark/mesh-io.h>
#include <iostream>
#include <fstream>
#include <map>
// #include <format>
#include <fmt/core.h>

namespace meshark {

void GeometryMesh::buildFromWavefrontObj(const WavefrontObj &obj) {
  for (auto v : obj.positions)
    createVertex(v);
  std::map<std::pair<int, int>, int> half_edge_indices{};
  for (int i = 0; i < obj.face_splits.size() - 1; i++) {
    int start = obj.face_splits[i];
    int end = obj.face_splits[i + 1];
    HalfEdge prev, starter;
    auto f = createFace(glm::vec3());
    for (int j = start; j < end; j++) {
      int nxt = j == end - 1 ? start : j + 1;
      auto h = createHalfEdge();
      int cur = obj.face_vertices[j].v;
      int next = obj.face_vertices[nxt].v;
      h->tail = vertex(cur);
      vertex(cur)->halfEdge() = h;
      h->tip = vertex(next);
      h->face = f;
      if (prev)
        prev->next = h;
      else starter = h;
      prev = h;
      assert(!half_edge_indices.contains({cur, next}));
      half_edge_indices[{cur, next}] = numHalfEdges() - 1;
      if (half_edge_indices.contains({next, cur})) {
        auto rev = halfEdge(half_edge_indices[{next, cur}]);
        h->twin = rev;
        h->edge = rev->edge;
        rev->twin = h;
      } else {
        auto e = createEdge();
        h->edge = e;
        e->halfEdge() = h;
      }
    }
    prev->next = starter;
    f->halfEdge() = starter;
    normals(f) = computeFaceNormal(f);
  }
}

void GeometryMesh::writeWavefrontObj(const std::filesystem::path &path) const {
  std::ofstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << path << std::endl;
    return;
  }
  for (auto v : vertices()) {
    auto p = position(v);
    file << fmt::format("v {} {} {}", p.x, p.y, p.z) << std::endl;
  }
  for (auto f : faces()) {
    file << "f ";
    for (auto h : f->boundaryHalfEdges()) {
      auto v = index(h->tail) + 1;
      file << v << " ";
    }
    file << "\n";
  }
  file.close();
}
}
