//
// Created by creeper on 8/2/24.
//

#ifndef MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_SET_H_
#define MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_SET_H_
#include <meshark/element-decl.h>
#include <vector>
#include <algorithm>

namespace meshark {
template <MeshElement Element>
struct MeshElementSet {
  MeshElementSet() = default;
  explicit MeshElementSet(const std::vector<Element>& elements) : m_elements(elements) {
    std::sort(m_elements.begin(), m_elements.end(), meshElementCompare);
  }
  static bool meshElementCompare(Element a, Element b) {
    return a.get() < b.get();
  }
  const std::vector<Element>& elements() const {
    return m_elements;
  }
  int size() const {
    return m_elements.size();
  }
  MeshElementSet computeIntersection(const MeshElementSet& rhs) const {
    std::vector<Element> temp;
    std::set_intersection(m_elements.begin(), m_elements.end(),
                          rhs.m_elements.begin(), rhs.m_elements.end(),
                          std::back_inserter(temp), meshElementCompare);
    return MeshElementSet(temp);
  }
 private:
  std::vector<Element> m_elements{};
};

using VertexSet = MeshElementSet<Vertex>;
using EdgeSet = MeshElementSet<Edge>;
using HalfEdgeSet = MeshElementSet<HalfEdge>;
using FaceSet = MeshElementSet<Face>;
}
#endif //MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_SET_H_
