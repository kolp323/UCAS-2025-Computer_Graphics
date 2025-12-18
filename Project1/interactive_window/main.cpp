#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

const GLsizei windowWidth = 800;
const GLsizei windowHeight = 800;
#define MAX_SHAPES 50
// 绘图模式
enum {DRAW_LINE, DRAW_TRIANGLE, DRAW_RECTANGLE};
// 颜色选择
enum{COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};

// 形状结构体
typedef struct {
    int mode;       // 形状
    float color[3]; // R, G, B
    float x1, y1;   // 起点
    float x2, y2;   // 终点
} Shape;

int currentMode = DRAW_LINE;
float currentColor[3] = {0.0f, 0.0f, 0.0f}; // 当前绘图颜色 
float startX = 0.0f, startY = 0.0f; 
float endX = 0.0f, endY = 0.0f; 
Shape shapes[MAX_SHAPES]; // 存储已绘制的图形
int numShapes = 0; // 已绘制图形的数量


void draw_line(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
}

void draw_rec(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x1, y2);
        glVertex2f(x2, y2);
        glVertex2f(x2, y1);
    glEnd();
}

void draw_triangle(float x1, float y1, float x2, float y2) {
    glBegin(GL_TRIANGLES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2); 
    glEnd();
}


void drawShape(Shape s) {
    glColor3f(s.color[0], s.color[1], s.color[2]);
    switch (s.mode) {
        case DRAW_LINE:
            draw_line(s.x1, s.y1, s.x2, s.y2);
            break;
        case DRAW_TRIANGLE:
            draw_triangle(s.x1, s.y1, s.x2, s.y2);
            break;
        case DRAW_RECTANGLE:
            draw_rec(s.x1, s.y1, s.x2, s.y2);
            break;
    }
}

void mouseCallback(int button, int state, int x, int y) {
    float gl_x = x * 2.0f / windowWidth - 1.0f;
    float gl_y = 1.0f - y * 2.0f / windowHeight;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // 记录绘图起点
            startX = gl_x;
            startY = gl_y;
        } else if (state == GLUT_UP) {
            // 鼠标松开：将当前图形保存到数组中
            if (numShapes < MAX_SHAPES) {
                shapes[numShapes].mode = currentMode;
                shapes[numShapes].color[0] = currentColor[0];
                shapes[numShapes].color[1] = currentColor[1];
                shapes[numShapes].color[2] = currentColor[2];
                shapes[numShapes].x1 = startX;
                shapes[numShapes].y1 = startY;
                shapes[numShapes].x2 = gl_x;
                shapes[numShapes].y2 = gl_y;
                numShapes++;
            }
            // 清除终点，防止松开后仍然显示
            endX = startX; 
            endY = startY;
            glutPostRedisplay();
        }
    }
}

void motionCallback(int x, int y) {
    endX = x * 2.0f / windowWidth - 1.0f;
    endY = 1.0f - y * 2.0f / windowHeight;
    glutPostRedisplay(); // 强制窗口重绘，显示拖拽效果
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制所有已存储的图形
    for (int i = 0; i < numShapes; i++) {
        drawShape(shapes[i]);
    }

    // 绘制当前正在拖拽的临时图形 (实时反馈)
    if (startX != endX || startY != endY) {
        drawShape((Shape){
            currentMode,
            {currentColor[0], currentColor[1], currentColor[2]},
            startX, startY, endX, endY
        });
    }

    glFlush();
}

void colorSelect(int choice) {
    switch (choice) {
        case COLOR_RED: currentColor[0] = 1.0f; currentColor[1] = 0.0f; currentColor[2] = 0.0f; break;
        case COLOR_GREEN: currentColor[0] = 0.0f; currentColor[1] = 1.0f; currentColor[2] = 0.0f; break;
        case COLOR_BLUE: currentColor[0] = 0.0f; currentColor[1] = 0.0f; currentColor[2] = 1.0f; break;
        case COLOR_YELLOW: currentColor[0] = 1.0f; currentColor[1] = 1.0f; currentColor[2] = 0.0f; break;
    }
}

void modeSelect(int choice) {
    currentMode = choice;
}

void mainSelect(int choice) {
    if (choice == 2) { // 'Clear' 选项的 ID 为 2
        numShapes = 0; // 清除所有存储的形状
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init_menu() {
    // 创建颜色子菜单
    int color_menu = glutCreateMenu(colorSelect);
    glutAddMenuEntry("red", COLOR_RED);
    glutAddMenuEntry("green", COLOR_GREEN);
    glutAddMenuEntry("blue", COLOR_BLUE);
    glutAddMenuEntry("yellow", COLOR_YELLOW);

    // 创建模式子菜单
    int mode_menu = glutCreateMenu(modeSelect);
    glutAddMenuEntry("draw lines", DRAW_LINE);
    glutAddMenuEntry("draw triangles", DRAW_TRIANGLE);
    glutAddMenuEntry("draw rectangles", DRAW_RECTANGLE);

    // 创建主菜单，连接子菜单和 Clear 选项
    glutCreateMenu(mainSelect); 
    glutAddSubMenu("Choose Mode", mode_menu);
    glutAddSubMenu("Choose Color", color_menu);
    glutAddMenuEntry("Clear", 2); // '2' 是用于 mainSelect 的 ID

    // 绑定到鼠标右键
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// --- Main ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Interactive Control Drawing Experiment");
    
    init();
    init_menu();
    
    // 注册鼠标和拖拽回调函数
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}