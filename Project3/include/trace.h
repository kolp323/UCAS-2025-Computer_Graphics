#ifndef TRACE_H
#define TRACE_H
#include <vector>
#include "element.h"
#define MAX_RAY_DEPTH 5

Vec3f trace(
    const Vec3f &rayorig, 
    const Vec3f &raydir, 
    const std::vector<Sphere> &spheres, 
    const int &depth
);

// 相机位置、目标点、FOV参数
void render(
    const std::vector<Sphere> &spheres
);

void renderToBuffer(
    const std::vector<Sphere> &spheres, 
    const Vec3f &camPos, 
    const Vec3f &camTarget, 
    float fov, 
    Vec3f *buffer
);

void save_frame(Vec3f* image, unsigned width, unsigned height, const char *outdir);
#endif