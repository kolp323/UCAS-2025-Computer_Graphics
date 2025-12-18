//
// Created by creeper on 7/21/24.
//
#include <meshark/mesh-io.h>
#include <fstream>
#include <iostream>

namespace meshark {

std::unique_ptr<WavefrontObj> readWavefrontObj(const std::filesystem::path &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << path << std::endl;
    return {};
  }

  std::string line;
  auto obj = std::make_unique<WavefrontObj>();
  std::string face_vertex;
  std::string prefix;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    iss >> prefix;

    if (prefix == "v") {
      glm::vec3 pos;
      iss >> pos.x >> pos.y >> pos.z;
      obj->positions.push_back(pos);
    } else if (prefix == "vt") {
      glm::vec2 uv;
      iss >> uv.x >> uv.y;
      obj->uvs.push_back(uv);
    } else if (prefix == "vn") {
      glm::vec3 normal;
      iss >> normal.x >> normal.y >> normal.z;
      obj->normals.push_back(normal);
    } else if (prefix == "f") {
      obj->face_splits.push_back(obj->face_vertices.size());
      while (iss >> face_vertex) {
        WavefrontObj::FaceVertex fv;
        std::replace(face_vertex.begin(), face_vertex.end(), '/', ' ');
        std::istringstream vertex_stream(face_vertex);
        int v_index, vt_index = -1, vn_index = -1;
        vertex_stream >> v_index;
        if (vertex_stream.peek() != EOF) vertex_stream >> vt_index;
        if (vertex_stream.peek() != EOF) vertex_stream >> vn_index;
        fv.v = v_index - 1;
        if (vt_index != -1) fv.vt = vt_index - 1;
        if (vn_index != -1) fv.vn = vn_index - 1;
        obj->face_vertices.push_back(fv);
      }
    }
  }
  obj->face_splits.push_back(obj->face_vertices.size());
  return obj;
}

std::unique_ptr<GeometryMesh> readGeometryMeshFromWavefrontObj(const std::filesystem::path &path) {
  auto obj = readWavefrontObj(path);
  if (!obj) return {};
  auto mesh = std::make_unique<GeometryMesh>();
  mesh->buildFromWavefrontObj(*obj);
  return mesh;
}
}