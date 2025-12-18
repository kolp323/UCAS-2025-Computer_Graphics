//
// Created by creeper on 8/2/24.
//

#ifndef MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_DECL_H_
#define MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_DECL_H_
#include <mystl/observer-ptr.h>
namespace meshark {
struct VertexElement;
struct HalfEdgeElement;
struct FaceElement;
struct EdgeElement;

using Vertex = mystl::observer_ptr<VertexElement>;
using HalfEdge = mystl::observer_ptr<HalfEdgeElement>;
using Face = mystl::observer_ptr<FaceElement>;
using Edge = mystl::observer_ptr<EdgeElement>;

template <typename T>
concept MeshElement = std::is_same_v<T, Vertex> || std::is_same_v<T, Edge> || std::is_same_v<T, HalfEdge> || std::is_same_v<T, Face>;

}
#endif //MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_ELEMENT_DECL_H_
