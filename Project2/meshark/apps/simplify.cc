#include <iostream>
#include <meshark/mesh-simplifier.h>
#include <meshark/mesh-io.h>
using namespace meshark;

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <input obj path> <output obj path> <ratio>" << std::endl;
    return 1;
  }
  auto mesh = readGeometryMeshFromWavefrontObj(argv[1]);
  std::unique_ptr<MeshSimplifier> simplifier = std::make_unique<MeshSimplifier>(*mesh);
  simplifier->runSimplify(std::stod(argv[3]));
  mesh->writeWavefrontObj(argv[2]);
}