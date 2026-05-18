#include<windows.h>
#include <GL/glut.h>
#define PI 3.1416
#include<math.h>
#include<stdlib.h>


// movement variable
float bx = 0.0f;
float gx = 0.0f;
float birdX = 0.0f;
float birdWing = 0.02f;
float speed = 0.02f;
float cloudX = 0.0f;


// sunset
float sunY = 0.75f;
float sunSize = 0.16f;
float skyR = 255, skyG = 160, skyB = 60;
float skyTR = 255, skyTG = 210, skyTB = 125;


// animation control
bool movingDown = true; //sun

bool animationRunning = false;


// circle
void drawCircle(float cx, float cy, float r, int red, int green, int blue) {

    glBegin(GL_POLYGON);
    glColor3ub(red, green, blue);

    for (int i = 0; i <= 60; i++) {

        float angle = 2 * PI * i / 60;

        glVertex2f(cx + r * cos(angle),
                   cy + r * sin(angle));
    }

    glEnd();
}

// rectangle
void drawRect(float x1, float y1,
              float x2, float y2,
              int r, int g, int b) {

    glColor3ub(r, g, b);

    glBegin(GL_QUADS);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);

    glEnd();
}

// ellipse
void drawEllipse(float cx, float cy,
                 float rx, float ry,
                 int r, int g, int b) {

    glColor3ub(r, g, b);

    glBegin(GL_POLYGON);

    for (int i = 0; i < 100; i++) {

        float angle = 2 * PI * i / 100;

        float x = rx * cos(angle);
        float y = ry * sin(angle);

        glVertex2f(cx + x, cy + y);
    }

    glEnd();
}

// boy
void drawBoy(float x, float y,
             int r, int g, int b)
{
    float s = 1.0f;

    drawEllipse(x, y + 0.22f * s, //head
                0.06f * s,
                0.08f * s,
                237, 200, 165);

    drawEllipse(x, y + 0.26f * s,//hair
                0.065f * s,
                0.045f * s,
                25, 25, 25);

    drawRect(x - 0.07f * s, //shirt
             y + 0.06f * s,
             x + 0.07f * s,
             y + 0.18f * s,
             r, g, b);

    drawRect(x - 0.10f * s,
             y + 0.10f * s,
             x - 0.07f * s,
             y + 0.16f * s,
             r, g, b);

    drawRect(x + 0.07f * s,
             y + 0.10f * s,
             x + 0.10f * s,
             y + 0.16f * s,
             r, g, b);

    drawRect(x - 0.10f * s,
             y + 0.08f * s,
             x - 0.06f * s,
             y + 0.12f * s,
             237, 200, 165);

    drawRect(x + 0.06f * s,
             y + 0.08f * s,
             x + 0.10f * s,
             y + 0.12f * s,
             237, 200, 165);

    drawRect(x - 0.06f * s,
             y - 0.08f * s,
             x + 0.06f * s,
             y + 0.06f * s,
             30, 30, 40);

    drawRect(x - 0.055f * s,
             y - 0.22f * s,
             x - 0.015f * s,
             y - 0.08f * s,
             30, 30, 40);

    drawRect(x + 0.015f * s,
             y - 0.22f * s,
             x + 0.055f * s,
             y - 0.08f * s,
             30, 30, 40);

    drawRect(x - 0.060f * s,
             y - 0.24f * s,
             x - 0.005f * s,
             y - 0.22f * s,
             10, 10, 10);

    drawRect(x + 0.005f * s,
             y - 0.24f * s,
             x + 0.060f * s,
             y - 0.22f * s,
             10, 10, 10);
}


void drawGirl(float x, float y)
{
    float s = 1.0f;

    drawEllipse(x, y + 0.22f * s,
                0.06f * s,
                0.08f * s,
                237, 200, 165);

    drawEllipse(x, y + 0.24f * s,
                0.070f * s,
                0.055f * s,
                20, 15, 15);

    drawRect(x - 0.070f * s,
             y + 0.12f * s,
             x - 0.030f * s,
             y + 0.24f * s,
             20, 15, 15);

    drawRect(x + 0.030f * s,
             y + 0.12f * s,
             x + 0.070f * s,
             y + 0.24f * s,
             20, 15, 15);

    drawRect(x - 0.065f * s,
             y + 0.10f * s,
             x + 0.065f * s,
             y + 0.18f * s,
             220, 70, 120);

    glColor3ub(170, 25, 70);

    glBegin(GL_QUADS);

    glVertex2f(x - 0.09f * s, y + 0.10f * s);
    glVertex2f(x + 0.09f * s, y + 0.10f * s);
    glVertex2f(x + 0.06f * s, y - 0.10f * s);
    glVertex2f(x - 0.06f * s, y - 0.10f * s);

    glEnd();

    drawRect(x - 0.10f * s,
             y + 0.10f * s,
             x - 0.06f * s,
             y + 0.14f * s,
             237, 200, 165);

    drawRect(x + 0.06f * s,
             y + 0.10f * s,
             x + 0.10f * s,
             y + 0.14f * s,
             237, 200, 165);

    drawRect(x - 0.030f * s,
             y - 0.22f * s,
             x - 0.005f * s,
             y - 0.10f * s,
             237, 200, 165);

    drawRect(x + 0.005f * s,
             y - 0.22f * s,
             x + 0.030f * s,
             y - 0.10f * s,
             237, 200, 165);

    drawRect(x - 0.035f * s,
             y - 0.24f * s,
             x,
             y - 0.22f * s,
             50, 0, 0);

    drawRect(x,
             y - 0.24f * s,
             x + 0.035f * s,
             y - 0.22f * s,
             50, 0, 0);
}

// cloud
void drawCloud(float x, float y) {

    drawCircle(x - 0.18, y, 0.06, 210, 210, 210);
    drawCircle(x - 0.10, y + 0.04, 0.08, 210, 210, 210);
    drawCircle(x, y + 0.06, 0.10, 210, 210, 210);
    drawCircle(x + 0.10, y + 0.04, 0.08, 210, 210, 210);
    drawCircle(x + 0.18, y, 0.06, 210, 210, 210);

    drawCircle(x - 0.20, y + 0.01, 0.07, 255, 255, 255);
    drawCircle(x - 0.12, y + 0.035, 0.075, 255, 255, 255);
    drawCircle(x, y + 0.045, 0.09, 255, 255, 255);
    drawCircle(x + 0.12, y + 0.035, 0.075, 255, 255, 255);
    drawCircle(x + 0.20, y + 0.01, 0.07, 255, 255, 255);

    drawCircle(x - 0.08, y - 0.01, 0.07, 255, 255, 255);
    drawCircle(x + 0.08, y - 0.01, 0.07, 255, 255, 255);

    glBegin(GL_QUADS);

    glColor3ub(255, 255, 255);

    glVertex2f(x - 0.22, y - 0.05);
    glVertex2f(x + 0.22, y - 0.05);
    glVertex2f(x + 0.22, y + 0.03);
    glVertex2f(x - 0.22, y + 0.03);

    glEnd();
}

// stars
void drawStars() {

    if (sunY < -0.1) {

        glBegin(GL_POINTS);

        glColor3ub(255, 255, 255);

        srand(123);

        for (int i = 0; i < 50; i++) {

            float x = (float)(rand() % 200 - 100) / 100.0f;
            float y = (float)(rand() % 100) / 100.0f;

            glVertex2f(x, y);
        }

        glEnd();
    }
}

// heart
void drawHeart(float x, float y, float size) {

    glBegin(GL_POLYGON);

    glColor3ub(220, 20, 60);

    for (float i = 0; i < 2 * PI; i += 0.01) {

        float hx = size * 16 * pow(sin(i), 3);

        float hy = size * (13 * cos(i) - 5 * cos(2 * i)
            - 2 * cos(3 * i) - cos(4 * i));

        glVertex2f(hx + x, hy + y);
    }

    glEnd();
}

// bird
void drawBird(float x, float y) {

    if (sunY < -0.15)
        return;

    glColor3ub(0, 0, 0);

    glLineWidth(2.0);

    glBegin(GL_LINE_STRIP);

    glVertex2f(x + birdX, y);
    glVertex2f(x + 0.05 + birdX, y + birdWing);
    glVertex2f(x + 0.1 + birdX, y);

    glEnd();
}

void controlSunset(float step) {

    if ((step < 0 && sunY > -0.5f) ||
        (step > 0 && sunY < 0.75f)) {

        sunY += step;

        if (step < 0) {

            if (skyG > 20) skyG -= 1.0;
            if (skyB > 20) skyB -= 0.5;

            if (skyTR > 20) skyTR -= 1.5;
            if (skyTG > 10) skyTG -= 1.5;
            if (skyTB > 40) skyTB -= 0.5;

            if (sunSize > 0.05f)
                sunSize -= 0.0015f;
        }

        else {

            if (skyG < 160) skyG += 1.0;
            if (skyB < 60) skyB += 0.5;

            if (skyTR < 255) skyTR += 1.5;
            if (skyTG < 210) skyTG += 1.5;
            if (skyTB < 120) skyTB += 0.5;

            if (sunSize < 0.16f)
                sunSize += 0.0015f;
        }
    }
}

// movement
void update(int value) {

    birdX += 0.008f;

    if (birdX > 1.2f)
        birdX = -1.2f;

    birdWing = (birdWing == 0.02f) ? -0.02f : 0.02f;

    cloudX += 0.004f;

    if (cloudX > 1.5f)
        cloudX = -1.5f;

    if (animationRunning) {

        if (movingDown) {

            controlSunset(-0.008f);

            if (sunY <= -0.45f)
                movingDown = false;
        }

        else {

            controlSunset(0.008f);

            if (sunY >= 0.75f)
                animationRunning = false;
        }
    }

    glutPostRedisplay();

    glutTimerFunc(30, update, 0);
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    // sky
    glBegin(GL_QUADS);

    glColor3ub(skyR, skyG, skyB);

    glVertex2f(-1.0, 0.0);
    glVertex2f(1.0, 0.0);

    glColor3ub(skyTR, skyTG, skyTB);

    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);

    glEnd();

    // clouds
    if (sunY > -0.1) {

        drawCloud(-0.5 + cloudX, 0.75);
        drawCloud(0.3 + cloudX, 0.65);

    }

    // stars
    drawStars();

    // mountains
    glBegin(GL_TRIANGLES);

    glColor3ub(80, 50, 30);

    glVertex2f(-1.1, 0.0);
    glVertex2f(-0.3, 0.5);
    glVertex2f(0.5, 0.0);

    glColor3ub(100, 70, 40);

    glVertex2f(-0.7, 0.0);
    glVertex2f(0.1, 0.6);
    glVertex2f(0.8, 0.0);

    glColor3ub(90, 60, 35);

    glVertex2f(0.2, 0.0);
    glVertex2f(0.7, 0.45);
    glVertex2f(1.2, 0.0);

    glEnd();

    // sun
    if (sunY > -0.35) {

        drawCircle(0.75, sunY, sunSize,
                   255, 140, 45);

        drawCircle(0.75, sunY,
                   sunSize * 0.7,
                   255, 100, 0);
    }
     //  moon
if (sunY < -0.1) {


    drawCircle(-0.75, 0.72, 0.08,
               255, 240, 120);


    drawCircle(-0.71, 0.73, 0.08,
               10,10,30);


}

    // birds
    drawBird(-0.7, 0.85);
    drawBird(-0.5, 0.78);
    drawBird(-0.3, 0.82);
    drawBird(0.2, 0.88);
    drawBird(0.4, 0.75);

    // grass
    glBegin(GL_QUADS);

    int gC = (sunY > -0.2) ? 150 : 60;

    glColor3ub(40, gC, 70);

    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 0.0);
    glVertex2f(-1.0, 0.0);

    glEnd();




    // road
    drawCircle(0.0, -0.9, 0.9,
               175, 165, 145);

    // flowers and leaves

// flower 1
drawCircle(-0.55, -0.72, 0.015, 255, 120, 180);
drawCircle(-0.53, -0.72, 0.015, 255, 120, 180);
drawCircle(-0.54, -0.70, 0.015, 255, 120, 180);
drawCircle(-0.54, -0.74, 0.015, 255, 120, 180);
drawCircle(-0.54, -0.72, 0.010, 255, 220, 0);

// flower 2
drawCircle(0.45, -0.68, 0.015, 255, 150, 150);
drawCircle(0.47, -0.68, 0.015, 255, 150, 150);
drawCircle(0.46, -0.66, 0.015, 255, 150, 150);
drawCircle(0.46, -0.70, 0.015, 255, 150, 150);
drawCircle(0.46, -0.68, 0.010, 255, 220, 0);

// leaf 1
glColor3ub(60, 140, 60);

glBegin(GL_POLYGON);

glVertex2f(-0.20, -0.78);
glVertex2f(-0.16, -0.76);
glVertex2f(-0.12, -0.78);
glVertex2f(-0.16, -0.81);

glEnd();

// leaf 2
glBegin(GL_POLYGON);

glVertex2f(0.18, -0.74);
glVertex2f(0.22, -0.72);
glVertex2f(0.26, -0.74);
glVertex2f(0.22, -0.77);

glEnd();

// leaves
glBegin(GL_POLYGON);

glVertex2f(0.60, -0.80);
glVertex2f(0.63, -0.79);
glVertex2f(0.66, -0.80);
glVertex2f(0.63, -0.82);

glEnd();
    // trees
    glColor3ub(100, 60, 30);

    glBegin(GL_QUADS);

    glVertex2f(-0.72, 0.1);
    glVertex2f(-0.68, 0.1);
    glVertex2f(-0.68, -0.2);
    glVertex2f(-0.72, -0.2);

    glVertex2f(0.68, 0.1);
    glVertex2f(0.72, 0.1);
    glVertex2f(0.72, -0.2);
    glVertex2f(0.68, -0.2);

    glEnd();

    drawCircle(-0.7, 0.28, 0.22,
               55, 150, 90);

    drawCircle(-0.88, 0.18, 0.18,
               50, 140, 80);

    drawCircle(-0.52, 0.18, 0.18,
               50, 140, 80);

    drawCircle(0.7, 0.28, 0.22,
               55, 150, 90);

    drawCircle(0.88, 0.18, 0.18,
               50, 140, 80);

    drawCircle(0.52, 0.18, 0.18,
               50, 140, 80);

    // bench
    glColor3ub(70, 35, 10);

    glBegin(GL_QUADS);

    glVertex2f(-0.2, -0.32);
    glVertex2f(0.2, -0.32);
    glVertex2f(0.2, -0.36);
    glVertex2f(-0.2, -0.36);

    glVertex2f(-0.2, -0.18);
    glVertex2f(0.2, -0.18);
    glVertex2f(0.2, -0.22);
    glVertex2f(-0.2, -0.22);

    glVertex2f(-0.18, -0.22);
    glVertex2f(-0.16, -0.22);
    glVertex2f(-0.16, -0.32);
    glVertex2f(-0.18, -0.32);

    glVertex2f(0.16, -0.22);
    glVertex2f(0.18, -0.22);
    glVertex2f(0.18, -0.32);
    glVertex2f(0.16, -0.32);

    glVertex2f(-0.17, -0.36);
    glVertex2f(-0.13, -0.36);
    glVertex2f(-0.13, -0.48);
    glVertex2f(-0.17, -0.48);

    glVertex2f(0.13, -0.36);
    glVertex2f(0.17, -0.36);
    glVertex2f(0.17, -0.48);
    glVertex2f(0.13, -0.48);

    glEnd();

    // boy
    drawBoy(-0.8 + bx, -0.45, 40, 80, 200);


     //girl
     drawGirl(0.8 + gx, -0.45);


    // heart logic
    float boyPos = -0.8f + bx;
    float girlPos = 0.8f + gx;

    float distance = girlPos - boyPos;

    if (distance < 0.25f) {

        animationRunning = true;

        drawHeart((boyPos + girlPos) / 2.0,
                  -0.1,
                  0.008);
    }

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {

    switch (key) {

    case 'd':
        bx += 0.04f;
        break;

    case 'a':
        bx -= 0.04f;
        break;

    case 'l':
        gx += 0.04f;
        break;

    case 'j':
        gx -= 0.04f;
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(600, 400);

    glutCreateWindow("Love Story - FIRST MEET");

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);

    glutTimerFunc(0, update, 0);

    glutMainLoop();

    return 0;
}
