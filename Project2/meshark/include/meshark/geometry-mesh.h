//
// Created by creeper on 7/21/24.
//

#ifndef MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_GEOMETRY_MESH_H_
#define MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_GEOMETRY_MESH_H_

#include <meshark/half-edge-mesh.h>
#include <meshark/element-data.h>
#include <glm/glm.hpp>
#include <filesystem>

namespace meshark {
struct WavefrontObj;

struct GeometryMesh : public HalfEdgeMesh<GeometryMesh> {
  GeometryMesh() = default;
  using Base = HalfEdgeMesh<GeometryMesh>;
  void buildFromWavefrontObj(const WavefrontObj &obj);
  void writeWavefrontObj(const std::filesystem::path &path) const;
  [[nodiscard]] glm::vec3 pos(Vertex v) const {
    return position(v);
  }
  [[nodiscard]] glm::vec3 normal(Face f) const {
    return normals(f);
  }
  void setVertexPos(Vertex v, const glm::vec3 &pos) {
    position(v) = pos;
    for (auto h : v->outgoingHalfEdges()) {
      auto f = h->face;
      normals(f) = computeFaceNormal(f);
    }
  }
 protected:
  friend Base;
  virtual void createVertexAttribute(const glm::vec3 &vertex_pos) {
    position.addVertexData(vertex_pos);
  }
  virtual void createVertexAttribute(float x, float y, float z) {
    position.addVertexData({x, y, z});
  }
  virtual void removeVertexAttribute(Vertex v) {
    position.removeVertexData(v);
  }
  virtual void createFaceAttribute(const glm::vec3 &face_normal) {
    normals.addFaceData(face_normal);
  }
  virtual void createFaceAttribute(float x, float y, float z) {
    normals.addFaceData({x, y, z});
  }
  virtual void removeFaceAttribute(Face f) {
    normals.removeFaceData(f);
  }
  [[nodiscard]] glm::vec3 computeFaceNormal(Face f) const {
    auto h = f->halfEdge();
    auto v0 = pos(h->tail);
    auto v1 = pos(h->tip);
    auto v2 = pos(h->next->tip);
    return glm::normalize(glm::cross(v1 - v0, v2 - v0));
  }
  FaceData<glm::vec3> normals;
  VertexData<glm::vec3> position;
};

template<>
struct additional_vertex_attribute_trait<GeometryMesh> {
  static constexpr bool value = true;
};
template<>
struct additional_face_attribute_trait<GeometryMesh> {
  static constexpr bool value = true;
};
}
#endif //MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_GEOMETRY_MESH_H_
