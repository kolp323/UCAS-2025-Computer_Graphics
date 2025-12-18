//
// Created by creeper on 7/21/24.
//

#ifndef MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_MESH_TYPE_TRAITS_H_
#define MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_MESH_TYPE_TRAITS_H_
namespace meshark {
template <typename T>
struct additional_vertex_attribute_trait {
  static constexpr bool value = false;
};
template <typename T>
struct additional_edge_attribute_trait {
  static constexpr bool value = false;
};
template <typename T>
struct additional_face_attribute_trait {
  static constexpr bool value = false;
};
}
#endif //MESHSIMPLIFICATION_MESH_SIMPLIFICATION_INCLUDE_MESH_SIMPLIFICATION_MESH_TYPE_TRAITS_H_
