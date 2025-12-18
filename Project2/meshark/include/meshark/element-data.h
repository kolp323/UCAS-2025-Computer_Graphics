//
// Created by creeper on 7/23/24.
//

#ifndef MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_DATA_H_
#define MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_DATA_H_
#include <vector>
#include <meshark/half-edge-mesh.h>
namespace meshark {
template <typename Type>
struct VertexData {
  using T = std::conditional_t<std::is_same_v<Type, bool>, std::byte, Type>;
  VertexData() = default;
  explicit VertexData(int num_vertices) : data(num_vertices) {}
  T& operator()(Vertex v) {
    return data[v->index];
  }
  const T& operator()(Vertex v) const {
    return data[v->index];
  }
  void addVertexData(const T& t) {
    data.push_back(t);
  }
  void removeVertexData(Vertex v) {
    if (v->index == data.size() - 1) {
      data.pop_back();
      return;
    }
    std::swap(data[v->index], data.back());
    data.pop_back();
  }
 private:
  std::vector<T> data{};
};
template <typename Type>
struct EdgeData {
  using T = std::conditional_t<std::is_same_v<Type, bool>, std::byte, Type>;
  EdgeData() = default;
  explicit EdgeData(int num_faces) : data(num_faces) {}
  T& operator()(Edge e) {
    return data[e->index];
  }
  const T& operator()(Edge e) const {
    return data[e->index];
  }
  void addEdgeData(const T& t) {
    data.push_back(t);
  }
  void removeEdgeData(Edge e) {
    if (e->index == data.size() - 1) {
      data.pop_back();
      return;
    }
    std::swap(data[e->index], data.back());
    data.pop_back();
  }
 private:
  std::vector<T> data{};
};
template <typename Type>
struct FaceData {
  using T = std::conditional_t<std::is_same_v<Type, bool>, std::byte, Type>;
  FaceData() = default;
  explicit FaceData(int num_faces) : data(num_faces) {}
  T& operator()(Face f) {
    return data[f->index];
  }
  const T& operator()(Face f) const {
    return data[f->index];
  }
  void addFaceData(const T& t) {
    data.push_back(t);
  }
  void removeFaceData(Face f) {
    if (f->index == data.size() - 1) {
      data.pop_back();
      return;
    }
    std::swap(data[f->index], data.back());
    data.pop_back();
  }
 private:
  std::vector<T> data{};
};
}
#endif //MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_DATA_H_
