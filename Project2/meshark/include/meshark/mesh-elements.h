//
// Created by creeper on 8/2/24.
//

#ifndef MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_MESH_ELEMENTS_H_
#define MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_MESH_ELEMENTS_H_

#include <meshark/element-decl.h>
#include <meshark/element-set.h>
#include <cassert>

namespace meshark {
struct HalfEdgeElement {
  explicit HalfEdgeElement(int index) : index(index) {}
  Vertex tip;
  Vertex tail;
  HalfEdge next;
  HalfEdge twin;
  Face face;
  Edge edge;
 protected:
  template<typename Derived>
  friend
  struct HalfEdgeMesh;
  int index;
};

inline HalfEdge nullHalfEdge() {
  return mystl::make_observer<HalfEdgeElement>(nullptr);
}

struct EdgeElement {
  explicit EdgeElement(int index) : index(index) {}

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
  template<typename Derived>
  friend
  struct HalfEdgeMesh;
  template<typename T>
  friend
  struct EdgeData;
  HalfEdge he;
  int index;
};

inline Edge nullEdge() {
  return mystl::make_observer<EdgeElement>(nullptr);
}

struct FaceElement {
 protected:
  struct BoundaryLoop {
    explicit BoundaryLoop(HalfEdge start) : start(start) {}

    struct Iterator {
      Iterator &operator++() {
        it = it->next;
        if (it == start) it = static_cast<HalfEdge>(nullptr);
        return *this;
      }

      HalfEdge operator*() const {
        return it;
      }

      bool operator==(const Iterator &other) const {
        return it == other.it;
      }

      HalfEdge start;
      HalfEdge it;
    };

    HalfEdge start;

    [[nodiscard]] Iterator begin() const {
      return {
          .start = start,
          .it = start,
      };
    }

    [[nodiscard]] Iterator end() const {
      return {
          .start = start,
          .it = static_cast<HalfEdge>(nullptr)
      };
    }
  };

 public:
  explicit FaceElement(int index) : index(index) {}

  [[nodiscard]] HalfEdge halfEdge() const {
    return he;
  }

  HalfEdge &halfEdge() {
    return he;
  }

  [[nodiscard]] BoundaryLoop boundaryHalfEdges() const {
    return BoundaryLoop(he);
  }

  [[nodiscard]] Vertex vertex() const {
    return he->tip;
  }

  [[nodiscard]] Edge edge() const {
    return he->edge;
  }

 protected:
  template<typename Derived>
  friend
  struct HalfEdgeMesh;
  template<typename T>
  friend
  struct FaceData;
  HalfEdge he;
  int index;
};
inline Face nullFace() {
  return mystl::make_observer<FaceElement>(nullptr);
}
struct VertexElement {
 private:
  struct OutgoingHalfEdgeRange {
    explicit OutgoingHalfEdgeRange(HalfEdge start) : start(start) {}

    struct Iterator {
      Iterator &operator++() {
        // TODO: implement operator++ for OutgoingHalfEdgeRange::Iterator
        it = it->twin->next;
        if (it == start) it = static_cast<HalfEdge>(nullptr); // 如果回到了起点，置为空，表示结束
        return *this;
      }

      HalfEdge operator*() const {
        return it;
      }

      bool operator==(const Iterator &other) const {
        return it == other.it;
      }

      HalfEdge start;
      HalfEdge it;
    };

    HalfEdge start;

    [[nodiscard]] Iterator begin() const {
      return {
          .start = start,
          .it = start,
      };
    }

    [[nodiscard]] Iterator end() const {
      return {
          .start = start,
          .it = static_cast<HalfEdge>(nullptr)
      };
    }
  };

 public:
  explicit VertexElement(int index) : index(index) {}

  [[nodiscard]] HalfEdge halfEdge() const {
    return he;
  }

  HalfEdge &halfEdge() {
    return he;
  }

  [[nodiscard]] OutgoingHalfEdgeRange outgoingHalfEdges() const {
    return OutgoingHalfEdgeRange(he);
  }

  [[nodiscard]] int degree() const {
    int deg = 0;
    for (auto h : outgoingHalfEdges())
      deg++;
    return deg;
  }

  [[nodiscard]] VertexSet adjacentVertices() const {
    std::vector<Vertex> adj_vertices;
    adj_vertices.reserve(degree());
    for (auto h : outgoingHalfEdges())
      adj_vertices.emplace_back(h->tip);
    return VertexSet(adj_vertices);
  }

  [[nodiscard]] std::optional<HalfEdge> halfEdgeTo(Vertex v) const {
    for (auto h : outgoingHalfEdges()) {
      if (h->tip == v) return h;
    }
    return std::nullopt;
  }

 protected:
  template<typename Derived>
  friend
  struct HalfEdgeMesh;
  template<typename T>
  friend
  struct VertexData;
  HalfEdge he;
  int index;
};
inline Vertex nullVertex() {
  return mystl::make_observer<VertexElement>(nullptr);
}
}
#endif //MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_MESH_ELEMENTS_H_
