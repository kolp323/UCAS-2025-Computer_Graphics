#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0); // 黑色
    glMatrixMode(GL_PROJECTION);
    glOrtho(-5, 5, -5, 5, 5, 15);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

    return;
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT); // 将屏幕清空为指定的背景色（在init()中的glClearColor完成）
    glutWireTeapot(3);
    glFlush();

    return;
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv); // 初始化glut库
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE); // 定义窗口的渲染特性
    glutInitWindowPosition(0, 0); // 设置窗口在屏幕上的位置
    glutInitWindowSize(300, 300); // 设置窗口大小
    glutCreateWindow("OpenGL 3D View"); // 创建窗口并命名
    init();
    glutDisplayFunc(display); // 注册绘图处理函数

    glutMainLoop(); 
    // 程序就会进入无限事件循环：它接管控制权，不断检查操作系统发送来的事件（比如按键、鼠标点击或窗口调整），并调用你注册的适当回调函数。
    return 0;
}
