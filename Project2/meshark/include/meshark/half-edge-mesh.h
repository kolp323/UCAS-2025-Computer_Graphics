//
// Created by creeper on 7/20/24.
//

#ifndef MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_HALF_EDGE_MESH_H_
#define MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_HALF_EDGE_MESH_H_

#include <vector>
#include <memory>
#include <optional>
#include <meshark/mesh-type-traits.h>
#include <meshark/mesh-elements.h>
#include <iostream>
#include <cassert>

namespace meshark {

struct WavefrontObj;

template<typename Derived>
struct HalfEdgeMesh {
  template<typename... Args>
  Vertex createVertex(Args &&... args) {
    m_vertices.push_back(std::make_unique<VertexElement>(numVertices()));
    auto v = mystl::make_observer(m_vertices.back().get());
    if constexpr (additional_vertex_attribute_trait<Derived>::value)
      derived().createVertexAttribute(std::forward<Args...>(args...));
    return v;
  }

  template<typename... Args>
  Edge createEdge(Args &&... args) {
    m_edges.push_back(std::make_unique<EdgeElement>(numEdges()));
    auto e = mystl::make_observer(m_edges.back().get());
    if constexpr (additional_edge_attribute_trait<Derived>::value)
      derived().createEdgeAttribute(std::forward<Args...>(args...));
    return e;
  }

  template<typename... Args>
  Face createFace(Args &&... args) {
    m_faces.push_back(std::make_unique<FaceElement>(numFaces()));
    auto f = mystl::make_observer(m_faces.back().get());
    if constexpr (additional_face_attribute_trait<Derived>::value)
      derived().createFaceAttribute(std::forward<Args...>(args...));
    return f;
  }

  template<typename... Args>
  HalfEdge createHalfEdge(Args &&... args) {
    m_half_edges.push_back(std::make_unique<HalfEdgeElement>(numHalfEdges()));
    return mystl::make_observer(m_half_edges.back().get());;
  }

  void removeVertex(Vertex v) {
    int idx = v->index;
    if constexpr (additional_vertex_attribute_trait<Derived>::value)
      derived().removeVertexAttribute(v);
    if (idx == numVertices() - 1) {
      m_vertices.pop_back();
      return;
    }
    std::swap(m_vertices[idx], m_vertices.back());
    m_vertices.pop_back();
    m_vertices[idx]->index = idx;
  }

  void removeFace(Face f) {
    int idx = f->index;
    if constexpr (additional_face_attribute_trait<Derived>::value)
      derived().removeFaceAttribute(f);
    if (idx == numFaces() - 1) {
      m_faces.pop_back();
      return;
    }
    std::swap(m_faces[idx], m_faces.back());
    m_faces.pop_back();
    m_faces[idx]->index = idx;
  }

  void removeEdge(Edge e) {
    int idx = e->index;
    if constexpr (additional_edge_attribute_trait<Derived>::value)
      derived().removeEdgeAttribute(e);
    if (idx == numEdges() - 1) {
      m_edges.pop_back();
      return;
    }
    std::swap(m_edges[idx], m_edges.back());
    m_edges.pop_back();
    m_edges[idx]->index = idx;
  }

  void removeHalfEdge(HalfEdge he) {
    int idx = he->index;
    if (idx == numHalfEdges() - 1) {
      m_half_edges.pop_back();
      return;
    }
    std::swap(m_half_edges[idx], m_half_edges.back());
    m_half_edges.pop_back();
    m_half_edges[idx]->index = idx;
  }

  [[nodiscard]] size_t numVertices() const { return m_vertices.size(); }

  [[nodiscard]] size_t numEdges() const { return m_edges.size(); }

  [[nodiscard]] size_t numFaces() const { return m_faces.size(); }

  [[nodiscard]] size_t numHalfEdges() const { return m_half_edges.size(); }

  [[nodiscard]] auto edges() const {
    return ElementRange<EdgeElement>(m_edges);
  }

  [[nodiscard]] auto vertices() const {
    return ElementRange<VertexElement>(m_vertices);
  }

  [[nodiscard]] auto faces() const {
    return ElementRange<FaceElement>(m_faces);
  }

  [[nodiscard]] auto halfEdges() const {
    return ElementRange<HalfEdgeElement>(m_half_edges);
  }

  [[nodiscard]] int index(Vertex v) const {
    return v->index;
  }

  [[nodiscard]] int index(Edge e) const {
    return e->index;
  }

  [[nodiscard]] int index(Face f) const {
    return f->index;
  }

  [[nodiscard]] int index(HalfEdge h) const {
    return h->index;
  }
  [[nodiscard]] Face face(int i) const {
    return mystl::make_observer(m_faces[i].get());
  }
  [[nodiscard]] Vertex vertex(int i) const {
    return mystl::make_observer(m_vertices[i].get());
  }
  [[nodiscard]] bool isCollapsable(Edge e) const {
    auto h1 = e->halfEdge();
    auto h2 = h1->twin;
    auto v1 = h1->tail;
    auto v2 = h2->tail;
    auto shared_neighbours = v1->adjacentVertices().computeIntersection(v2->adjacentVertices());
    if (shared_neighbours.size() > 2)
      return false;
    return true;
  }
 protected:

  [[nodiscard]] Edge edge(int i) const {
    return mystl::make_observer(m_edges[i].get());
  }

  [[nodiscard]] HalfEdge halfEdge(int i) const {
    return mystl::make_observer(m_half_edges[i].get());
  }

  template<typename Element>
  struct ElementRange {
    explicit ElementRange(const std::vector<std::unique_ptr<Element>> &elements) : elements(elements) {}

    using ElementObserver = mystl::observer_ptr<Element>;

    struct Iterator {
      Iterator &operator++() {
        ++it;
        return *this;
      }

      ElementObserver operator*() const {
        return mystl::make_observer(it->get());
      }

      bool operator==(const Iterator &other) const {
        return it == other.it;
      }

      std::vector<std::unique_ptr<Element>>::const_iterator it;
    };

    [[nodiscard]] Iterator begin() const {
      return {
          .it = elements.begin()
      };
    }

    [[nodiscard]] Iterator end() const {
      return {
          .it = elements.end()
      };
    }

    const std::vector<std::unique_ptr<Element>> &elements;
  };

 private:
  std::vector<std::unique_ptr<EdgeElement>> m_edges;
  std::vector<std::unique_ptr<FaceElement>> m_faces;
  std::vector<std::unique_ptr<VertexElement>> m_vertices;
  std::vector<std::unique_ptr<HalfEdgeElement>> m_half_edges;

  [[nodiscard]] const Derived &derived() const {
    return *static_cast<Derived *>(this);
  }

  Derived &derived() {
    return *static_cast<Derived *>(this);
  }
};

}
#endif //MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_HALF_EDGE_MESH_H_
