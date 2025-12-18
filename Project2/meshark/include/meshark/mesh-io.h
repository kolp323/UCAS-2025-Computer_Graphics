//
// Created by creeper on 7/21/24.
//

#ifndef MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_MESH_IO_H_
#define MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_MESH_IO_H_

#include <meshark/geometry-mesh.h>
#include <filesystem>

namespace meshark {
struct WavefrontObj {
  struct FaceVertex {
    int v{-1};
    std::optional<int> vt;
    std::optional<int> vn;
  };
  std::vector<int> face_splits;
  std::vector<FaceVertex> face_vertices;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;
};

std::unique_ptr<WavefrontObj> readWavefrontObj(const std::filesystem::path &path);
std::unique_ptr<GeometryMesh> readGeometryMeshFromWavefrontObj(const std::filesystem::path &path);
}
#endif //MESHSIMPLIFICATION_MESHARK_INCLUDE_MESHARK_MESH_IO_H_
