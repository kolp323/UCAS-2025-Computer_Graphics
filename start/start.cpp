#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>

const GLsizei windowWidth = 800;
const GLsizei windowHeight = 800;

void draw_screen(){
    // 把背景设置为黑色
    glClearColor(0.0, 0.0, 0.0, 0.0); //把清除颜色设置为黑色
    glClear(GL_COLOR_BUFFER_BIT); // 把整个窗口清除为当前清除颜色

//++++++++++++++基本图元绘制+++++++++++++++++++++
    // ====基本图元：点
    glColor3f(1.0f, 0.0f, 1.0f);
    glPointSize(5); // 设置点的大小，单位：pixel
    glBegin(GL_POINTS);
        glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();

    // ====基本图元：线
    glColor3f(1.0f, 0.5f, 0.1f);
    glLineWidth(4); // 设置线宽，单位：pixel
    glBegin(GL_LINES);
        // 第一个点 (V1)
        glVertex3f(-0.5f, 0.5f, 0.0f); 
        // 第二个点 (V2) -> 组成第一条线
        glVertex3f(0.5f, -0.5f, 0.0f);
    glEnd();

    // ====基本图元：单个实心三角形
    // 卷绕顺序 (Winding Order)：通常默认使用 逆时针（Counter-Clockwise, CCW）顺序来定义正面
    glColor3f(1.0f, 0.5f, 0.8f);
    glBegin(GL_TRIANGLES);
        glVertex3f(-0.5f, 0.5f, 0.0f); 
        glVertex3f(0.5f, -0.5f, 0.0f);
        glVertex3f(0.5f, 0.5f, 0.0f);
    glEnd();

    // ====基本图元：三角形带（Triangle Strip）
    // 只需要 $N+2$ 个顶点就能画出 $N$ 个三角形
    // T1 (第一个三角形)：由前三个顶点 $(V_1, V_2, V_3)$ 构成。
    // T2 (第二个三角形)：由 $(V_2, V_3, V_4)$ 构成。
    // （注意：为了维持正确的卷绕顺序，OpenGL 内部会对后两个顶点进行切换，但从概念上理解，它就是复用了前两个顶点。）
    glColor3f(0.2f, 0.5f, 0.8f);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(-0.1f, 0.1f, 0.0f); 
        glVertex3f(0.1f, -0.1f, 0.0f);
        glVertex3f(0.1f, 0.1f, 0.0f);
        glVertex3f(0.1f, 0.2f, 0.0f);
        glVertex3f(0.2f, 0.1f, 0.0f);
    glEnd();

    // ====基本图元：三角形扇 (GL_TRIANGLE_FAN)
    // $V_1$ 永远是中心点。
    // 第一个三角形 ($T_1$) 由 $V_1, V_2, V_3$ 组成。
    // 第二个三角形 ($T_2$) 由 $V_1, V_3, V_4$ 组成。
    glColor3f(0.2f, 0.9f, 0.8f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.0f); 
        glVertex3f(0.0f, 1.0f, 0.0f); 
        glVertex3f(0.5f, 0.866f, 0.0f); 
        glVertex3f(0.866f, 0.5f, 0.0f); 
        glVertex3f(1.0f, 0.0f, 0.0f); 
    glEnd();


    // ====基本图元：闭合多段线（Line Loop）
    glColor3f(0.2f, 0.9f, 0.8f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(0.0f, 0.0f, 0.0f); 
        glVertex3f(0.0f, -1.0f, 0.0f); 
        glVertex3f(-0.5f, -0.866f, 0.0f); 
        glVertex3f(-0.866f, -0.5f, 0.0f); 
        glVertex3f(-1.0f, 0.0f, 0.0f); 
    glEnd();

    // ====基本图元：闭合多段线单个、实心四边形
    // 逆时针方向绘制
    // 四个顶点颜色不同时，opengl自动进行颜色插值
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.5f, 0.1f);
        glVertex3f(-0.5f, 0.5f, 0.0f); 
        glColor3f(1.0f, 0.5f, 0.8f);
        glVertex3f(-0.5f, 1.0f, 0.0f); 

        glColor3f(1.0f, 0.5f, 0.8f);
        glVertex3f(-1.0f, 1.0f, 0.0f); 
        glColor3f(1.0f, 0.5f, 0.1f);
        glVertex3f(-1.0f, 0.5f, 0.0f); 
    glEnd();

//++++++++++++++变换的使用+++++++++++++++++++++
    // 移动、旋转、缩放：选择模型视图矩阵 
    glMatrixMode(GL_MODELVIEW);
    // 重置矩阵为单位矩阵
    glLoadIdentity();

    // 刷新opengl命令队列：强制绘图
    glFlush();
    // glutSwapBuffers(); // 双缓冲区
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0); // 设置窗口在屏幕上的位置
    glutInitWindowSize(300, 300); // 设置窗口大小
    glutCreateWindow("screen");

    glutDisplayFunc(draw_screen);
    glutMainLoop();
    return 0;
}