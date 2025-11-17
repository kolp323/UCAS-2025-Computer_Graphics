#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h> 
#define PI 3.14159265358979323846
#define SQRT_3 1.732
#define GoldenRatio 0.618

const GLsizei windowWidth = 800;
const GLsizei windowHeight = 800;

const float MonitorWidth = 0.6f;
const float MonitorHeight = MonitorWidth * GoldenRatio;
const float MonitorPadding = 0.01f;
const float MonitorRadius = 0.025f;

const float CircleRadius = MonitorWidth / 14.0;

const float TriangleSide = MonitorWidth / 3.0;

const float StentWidth = 0.15f;
const float StentHeight = StentWidth * GoldenRatio;

const float BaseWidth = 0.3f;
const float BaseHeight = 0.1f;
const float BaseRadius = BaseHeight/4.0;

enum DIRECTION
{               // yx
    RIGHT_UP,   // 00
    LEFT_UP,    // 01
    RIGHT_DOWN, // 10
    LEFT_DOWN,  // 11
};

void init(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);  
    // 初始化投影设置：为了在 2D 模式下正确观察到整个绘图区域
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 定义观察范围
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    // 初始化模型视图矩阵: 
    // Modelview Matrix 相当于一张虚拟的坐标纸
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void draw_quater_circle(const float r, const int direction){
    const int num_segments = 16; // 用于近似圆弧的线段数
    // 逆时针画四分之一圆弧
    // 计算起始角度 (0=右上, 1=左上, 2=左下, 3=右下)
    float start_angle = direction * (PI / 2.0);

    glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= num_segments; i++) {
            // 计算当前角度（从起始角度开始，递增 PI/2）
            float current_angle = start_angle + (PI / 2.0 * i / num_segments);
            
            // 将极坐标转换为笛卡尔坐标
            float x = r * cos(current_angle);
            float y = r * sin(current_angle);
            
            glVertex2f(x, y);
        }
    glEnd();    

}

void draw_filled_circle(const float r){
    const int num_segments = 64; // 用于近似圆弧的线段数
    const float delta_angle = 2 * PI / num_segments;
    float current_angle = 0.0;
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.0f); // 中心点
        for (int i = 0; i <= num_segments; i++)
        {
            // 将极坐标转换为笛卡尔坐标
            float x = r * cos(current_angle);
            float y = r * sin(current_angle);
            glVertex2f(x, y);
            current_angle += delta_angle;
        }
    glEnd();

}

void draw_hollow_circle(const float r){
    draw_quater_circle(r, RIGHT_UP);
    draw_quater_circle(r, LEFT_UP);
    draw_quater_circle(r, RIGHT_DOWN);
    draw_quater_circle(r, LEFT_DOWN);
}

void draw_equilateral_triangle(const float side){
    const float CircumcircleRadius = side / SQRT_3;
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, CircumcircleRadius);
        glVertex2f(-side/2.0, -CircumcircleRadius/2.0);
        glVertex2f(side/2.0, -CircumcircleRadius/2.0);
    glEnd();
}

void draw_filled_triangle(const float side){
    const float CircumcircleRadius = side / SQRT_3;
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, CircumcircleRadius);
        glVertex2f(-side / 2.0, -CircumcircleRadius / 2.0);
        glVertex2f(side / 2.0, -CircumcircleRadius / 2.0);
    glEnd();
}

void draw_hollow_rec(const float w, const float h){
    glBegin(GL_LINE_LOOP);
        glVertex2f(w/2.0, h/2.0);
        glVertex2f(-w/2.0, h/2.0);
        glVertex2f(-w/2.0, -h/2.0);
        glVertex2f(w/2.0, -h/2.0);
    glEnd();
}

void draw_filled_rec(const float w, const float h){
    glBegin(GL_QUADS);
        glVertex2f(w/2.0, h/2.0);
        glVertex2f(-w/2.0, h/2.0);
        glVertex2f(-w/2.0, -h/2.0);
        glVertex2f(w/2.0, -h/2.0);
    glEnd();
}

void draw_filled_rounded_rec(const float w, const float h, const float r){
    const float half_w_line = w / 2.0 - r;
    const float half_h_line = h / 2.0 - r;
    draw_filled_rec(w - 2 * r, h);
    draw_filled_rec(w, h - 2 * r);
    glPushMatrix();
        glTranslatef(half_w_line, half_h_line, 0.0f);
        draw_filled_circle(r);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-half_w_line, half_h_line, 0.0f);
        draw_filled_circle(r);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-half_w_line, -half_h_line, 0.0f);
        draw_filled_circle(r);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(half_w_line, -half_h_line, 0.0f);
        draw_filled_circle(r);
    glPopMatrix();
}

void draw_hollow_rounded_rec(const float w, const float h, const float r){
    const float half_w_line = w / 2.0 - r;
    const float half_h_line = h / 2.0 - r;
    glBegin(GL_LINES);
        // 上
        glVertex2f(half_w_line, h/2.0);
        glVertex2f(-half_w_line, h/2.0);
        // 下
        glVertex2f(half_w_line, -h/2.0);
        glVertex2f(-half_w_line, -h/2.0);     
        // 左
        glVertex2f(-w/2.0, half_h_line);
        glVertex2f(-w/2.0, -half_h_line);
        // 右
        glVertex2f(w/2.0, half_h_line);
        glVertex2f(w/2.0, -half_h_line);
    glEnd();    

    // 绘制圆角
    glPushMatrix();
        glTranslatef(half_w_line, half_h_line, 0.0f);
        draw_quater_circle(r, RIGHT_UP);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-half_w_line, half_h_line, 0.0f);
        draw_quater_circle(r, LEFT_UP);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-half_w_line, -half_h_line, 0.0f);
        draw_quater_circle(r, RIGHT_DOWN);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(half_w_line, -half_h_line, 0.0f);
        draw_quater_circle(r, LEFT_DOWN);
    glPopMatrix();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);  // 清除缓冲区
    //===绘制显示器
    // 显示器底色
    glColor3f(0.5f, 0.5f, 0.5f);
    draw_filled_rec(MonitorWidth, MonitorHeight);
    // 显示器边框
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_hollow_rec(MonitorWidth, MonitorHeight);

    // 显示屏底色
    glColor3f(0.4f, 0.7f, 1.0f); // 莫兰迪蓝
    draw_filled_rounded_rec(
        MonitorWidth - 2 * MonitorPadding,
        MonitorHeight - 2 * MonitorPadding,
        MonitorRadius
    );
    // 显示屏边框
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_hollow_rounded_rec(
        MonitorWidth - 2 * MonitorPadding, 
        MonitorHeight - 2 * MonitorPadding, 
        MonitorRadius
    );
    
    // 三角形图案
    glColor3f(1.0f, 0.5f, 0.8f); // 粉色
    draw_filled_triangle(TriangleSide);
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_equilateral_triangle(TriangleSide);

    // 圆形图案
    glColor3f(0.8f, 0.6f, 1.0f); // 薰衣草紫
    draw_filled_circle(CircleRadius);
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_hollow_circle(CircleRadius);

    // 为了确保每个组件都是独立定位的，需要在进行变换之前保存原始坐标系统，并在绘制完成后恢复它。
    
    //===绘制底座
    glPushMatrix(); // 保存当前坐标系 (原点)
        // 移动绘图的起点: 将当前的 Modelview Matrix 乘以 一个表示平移的矩阵，移动了整张坐标纸
        glTranslatef(0.0f, -(MonitorHeight/2.0 + StentHeight), 0.0f);
        glColor3f(0.32f, 0.32f, 0.32f);
        draw_filled_rounded_rec(BaseWidth, BaseHeight, BaseRadius);
        glColor3f(0.0f, 0.0f, 0.0f);
        draw_hollow_rounded_rec(BaseWidth, BaseHeight, BaseRadius);
    glPopMatrix(); // 恢复到保存的坐标系 (回到原点)

    //===绘制支架
    glPushMatrix();
        glTranslatef(0.0f, -(MonitorHeight/2.0 + StentHeight/2.0), 0.0f);
        glColor3f(0.3f, 0.3f, 0.3f); 
        draw_filled_rec(StentWidth, StentHeight);
        glColor3f(0.0f, 0.0f, 0.0f); // 绘制黑色边框
        draw_hollow_rec(StentWidth, StentHeight);
    glPopMatrix(); // 恢复到保存的坐标系 (回到原点)

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0); // 设置窗口在屏幕上的位置
    glutInitWindowSize(windowWidth, windowHeight); // 设置窗口大小
    glutCreateWindow("screen");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}