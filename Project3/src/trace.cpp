#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "trace.h"
#include "kd_tree.h"
#include <fstream>

extern KDNode* g_kdRoot;

float mix(const float &a, const float &b, const float &mix) {
    return b * mix + a * (1 - mix);
}


Vec3f trace(const Vec3f &rayorig, const Vec3f &raydir, const std::vector<Sphere> &spheres, const int &depth) {
    float tnear = INFINITY; // 最近相交点距离
    // const Sphere* sphere = nullptr; // 最近相交球体

    // // 遍历所有球体寻找最近交点
    // for (unsigned i = 0; i < spheres.size(); ++i) {
    //     float t0 = INFINITY, t1 = INFINITY;
    //     if (spheres[i].intersect(rayorig, raydir, t0, t1)) {
    //         if (t0 < 0) t0 = t1; // 如果 t0 为负，尝试使用 t1
    //         if (t0 < tnear) {
    //             tnear = t0;
    //             sphere = &spheres[i];
    //         }
    //     }
    // }
    const Sphere* sphere = intersect_kd_tree(g_kdRoot, rayorig, raydir, tnear);

    // 如果没有撞上任何物体，返回背景颜色 白色
    if (!sphere) return Vec3f(2); 

    // 计算交点 P 和该点的法线 N
    Vec3f phit = rayorig + raydir * tnear; // 交点坐标
    Vec3f nhit = phit - sphere->center;    // 计算法线
    nhit.normalize();                      // 归一化法线
    
    // 处理反射和折射
    Vec3f surfaceColor = 0;
    float bias = 1e-4; // 偏移量，防止阴影粉刺（自相交）
    bool inside = false;
    if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true; // 处理光线从内部射出的情况

    // 反射/透明物体：计算表面颜色
    if ((sphere->transparency > 0 || sphere->reflectivity > 0) && depth < MAX_RAY_DEPTH) {
        float facingratio = -raydir.dot(nhit);
        // 菲涅耳公式的简化近似：角度越偏，反射越强
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);

        // 计算反射方向
        Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
        refldir.normalize();
        Vec3f reflection = trace(phit + nhit * bias, refldir, spheres, depth + 1);

        // 计算折射方向
        Vec3f refraction = 0;
        if (sphere->transparency > 0) {
            float ior = 1.1, eta = (inside) ? ior : 1 / ior; // 折射率
            float cos_i = -nhit.dot(raydir);
            float k = 1 - eta * eta * (1 - cos_i * cos_i);
            Vec3f refrdir = raydir * eta + nhit * (eta * cos_i - sqrt(k));
            refrdir.normalize();
            refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);
        }

        // 综合颜色结果
        surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
    }
    // 漫反射物体/达到最大深度 终止跟踪，计算阴影
    else {
        // 计算漫反射颜色
        for (unsigned i = 0; i < spheres.size(); ++i) {
            if (spheres[i].emissionColor.x > 0) {
                // 光源
                Vec3f transmission = 1;
                Vec3f lightVec = spheres[i].center - phit;
                float dToLight = lightVec.length();
                Vec3f lightDirection = lightVec / dToLight;

                // // 阴影射线：检查交点与光源之间是否有遮挡
                // for (unsigned j = 0; j < spheres.size(); ++j) {
                //     if (i != j) {
                //         float t0, t1;
                //         if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) {
                //             transmission = 0; // 被遮挡，进入阴影
                //             break;
                //         }
                //     }
                // }

                float tShadow = dToLight; // 初始距离设为到光源的距离
                const Sphere* shadowObj = intersect_kd_tree(g_kdRoot, phit + nhit * bias, lightDirection, tShadow);
                // 如果在到光源的距离(dToLight)内碰到了非光源物体，则是阴影
                if (shadowObj && shadowObj != &spheres[i]) {
                    transmission = 0;
                }
                // 漫反射计算：颜色 * 强度 * 夹角余弦
                surfaceColor += sphere->surfaceColor * transmission * std::max(0.0f, nhit.dot(lightDirection)) * spheres[i].emissionColor;
            }
        }
    }

    return surfaceColor + sphere->emissionColor;
}

void renderToBuffer(const std::vector<Sphere> &spheres, const Vec3f &camPos, const Vec3f &camTarget, float fov, Vec3f* buffer) {
    float invWidth = 1 / float(640), invHeight = 1 / float(480);
    float aspectratio = 640 / float(480);
    float angle = tan(M_PI * 0.5 * fov / 180.);

    // 计算相机基向量 u, v, w
    Vec3f vup(0, 1, 0);
    Vec3f w = (camPos - camTarget); w.normalize();
    
    // 自定义叉乘逻辑
    auto cross = [](const Vec3f &a, const Vec3f &b) {
        return Vec3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    };
    
    Vec3f u = cross(vup, w); u.normalize();
    Vec3f v = cross(w, u);

    for (unsigned y = 0; y < 480; ++y) {
        for (unsigned x = 0; x < 640; ++x) {
            float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            
            Vec3f raydir = u * xx + v * yy - w;
            raydir.normalize();
            
            // OpenGL 的像素起点在左下角，需要进行 y 轴翻转映射
            buffer[(480 - 1 - y) * 640 + x] = trace(camPos, raydir, spheres, 0);
        }
    }
}

void save_frame(Vec3f* image, unsigned width, unsigned height, const char *outdir) {
    static int save_num = 0; // 已保存的图片数

    // 构建文件名
    char filename[256];
    std::snprintf(filename, sizeof(filename), "%s/frame_%d.png", outdir, save_num++);

    // 将 Vec3f (float) 转换为 PNG 格式需要的 R8G8B8 (unsigned char)
    std::vector<unsigned char> pixels(width * height * 3);
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            // 从 image 缓冲区中反向读取 y 轴
            // imageBuffer 的 (height-1-y) 行对应 PNG 的第 y 行
            unsigned int src_idx = (height - 1 - y) * width + x;
            unsigned int dst_idx = (y * width + x) * 3;
            pixels[dst_idx + 0] = (unsigned char)(std::max(0.0f, std::min(1.0f, image[src_idx].x)) * 255);
            pixels[dst_idx + 1] = (unsigned char)(std::max(0.0f, std::min(1.0f, image[src_idx].y)) * 255);
            pixels[dst_idx + 2] = (unsigned char)(std::max(0.0f, std::min(1.0f, image[src_idx].z)) * 255);
        }
    }
    // 写入 PNG 文件
    // 参数：文件名, 宽, 高, 通道数(3为RGB), 数据指针, 每行字节数
    int success = stbi_write_png(filename, width, height, 3, pixels.data(), width * 3);
    if (success) {
        std::cout << "Saved: " << filename << std::endl;
    } else {
        std::cerr << "Failed to save: " << filename << std::endl;
    }
}