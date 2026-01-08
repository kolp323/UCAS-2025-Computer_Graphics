#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "element.h"
#include "trace.h"
#include "kd_tree.h"

unsigned g_width = 640;
unsigned g_height = 480;
std::vector<Sphere> g_spheres;
KDNode* g_kdRoot = nullptr;
Vec3f* g_imageBuffer = nullptr;
const char *outdir = "./output";

// 相机交互参数
Vec3f g_camPos(0, 0, 5);      // 相机位置
Vec3f g_camTarget(0, 0, -20); // 观察目标点
float g_fov = 30.0f;          // 视场角

// 将 Vec3f 缓冲区转换为 OpenGL 可用的像素字节流
void updateDisplayBuffer() {
    renderToBuffer(g_spheres, g_camPos, g_camTarget, g_fov, g_imageBuffer);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 将渲染好的图像绘制到屏幕
    // g_imageBuffer 存储的是 0-255 的 unsigned char RGB 数据
    glDrawPixels(g_width, g_height, GL_RGB, GL_FLOAT, g_imageBuffer);

    glutSwapBuffers();
}

// 交互控制
void keyboard(unsigned char key, int x, int y) {
    float step = 0.5f;
    switch (key) {
        case 'w': g_camPos.z -= step; break; // 前进
        case 's': g_camPos.z += step; break; // 后退
        case 'a': g_camPos.x -= step; break; // 左移
        case 'd': g_camPos.x += step; break; // 右移
        case 'r': g_camPos.y += step; break; // 上移
        case 'f': g_camPos.y -= step; break; // 下移
        case 'z': g_fov = std::max(5.0f, g_fov - 1.0f); break; // 缩小 FOV
        case 'x': g_fov = std::min(120.0f, g_fov + 1.0f); break; // 扩大 FOV
        case 'c': save_frame(g_imageBuffer, g_width, g_height, outdir); break;
        case 27:
            exit(0);
            break; // ESC 键退出
        }
    // 触发重新渲染
    updateDisplayBuffer();
    glutPostRedisplay();
}

void initScene() {
    g_spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.2), 0, 0.0));
    g_spheres.push_back(Sphere(Vec3f(0.0, 0, -20), 4, Vec3f(1.00, 0.32, 0.36), 1, 0.5)); 
    g_spheres.push_back(Sphere(Vec3f(5.0, -1, -15), 2, Vec3f(0.90, 0.76, 0.46), 1, 0.0));
    g_spheres.push_back(Sphere(Vec3f(5.0, 0, -25), 3, Vec3f(0.65, 0.77, 0.97), 1, 0.0));
    g_spheres.push_back(Sphere(Vec3f(-5.5, 0, -15), 3, Vec3f(0.90, 0.90, 0.90), 1, 0.0));
    // 光源
    g_spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0), 0, 0.0, Vec3f(1)));
    g_imageBuffer = new Vec3f[g_width * g_height];
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_width, g_height);
    glutCreateWindow("Ray Tracing Interactive Camera");

    initScene();
    std::vector<const Sphere*> sphere_ptrs;
    for (const auto& s : g_spheres) sphere_ptrs.push_back(&s);
    g_kdRoot = build_kd_tree(sphere_ptrs, 0);

    updateDisplayBuffer(); // 初次渲染
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    std::cout << "控制方式: W/S 前后, A/D 左右, R/F 上下, Z/X 缩放, C 保存渲染图" << std::endl;

    glutMainLoop();
    return 0;
}